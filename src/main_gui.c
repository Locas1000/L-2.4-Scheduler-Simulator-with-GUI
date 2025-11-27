#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

// --- Global State ---
GtkWidget *window;
GtkWidget *process_list_store;
GtkWidget *drawing_area;
GtkWidget *combo_algorithm;
GtkWidget *label_metrics; // New label to show text results

process_t processes[MAX_PROCESSES];
timeline_event_t timeline[1000]; // Max timeline size
int num_processes = 0;
int total_time = 0;

// Color palette for processes (RGB)
double colors[6][3] = {
    {0.8, 0.2, 0.2}, // Red
    {0.2, 0.6, 0.2}, // Green
    {0.2, 0.4, 0.8}, // Blue
    {0.8, 0.8, 0.2}, // Yellow
    {0.8, 0.5, 0.2}, // Orange
    {0.5, 0.2, 0.8}  // Purple
};

// --- Helper: Read Data from GUI Table ---
void fetch_data_from_gui() {
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(process_list_store), &iter);

    num_processes = 0;
    while (valid && num_processes < MAX_PROCESSES) {
        int pid, arr, burst, prio;
        gtk_tree_model_get(GTK_TREE_MODEL(process_list_store), &iter,
                           0, &pid, 1, &arr, 2, &burst, 3, &prio, -1);

        processes[num_processes].pid = pid;
        processes[num_processes].arrival_time = arr;
        processes[num_processes].burst_time = burst;
        processes[num_processes].priority = prio;
        processes[num_processes].remaining_time = burst; // Reset

        num_processes++;
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(process_list_store), &iter);
    }
}

// --- Drawing Callback (The Gantt Chart) ---
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    (void)data; (void)widget;

    if (total_time == 0) return FALSE;

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Scale: How many pixels per time unit?
    double scale = (double)(width - 20) / total_time;
    int bar_height = 40;
    int y_pos = (height / 2) - (bar_height / 2);

    // Draw Background
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_paint(cr);

    // Draw Timeline Events
    // Note: In a real app, we'd iterate the 'timeline' array.
    // Since we cleared timeline in run_simulation, we rely on the algorithms filling it.
    // For simplicity, we just redraw the whole timeline array we filled.

    // Re-calculate how many events we have (simple scan)
    int event_count = 0;
    for(int i=0; i<1000; i++) {
        if(timeline[i].duration == 0 && i > 0) break; // End of events
        // Draw this block
        int pid_idx = (timeline[i].pid) % 6; // Color cycling
        cairo_set_source_rgb(cr, colors[pid_idx][0], colors[pid_idx][1], colors[pid_idx][2]);

        double x = 10 + (timeline[i].time * scale);
        double w = timeline[i].duration * scale;

        cairo_rectangle(cr, x, y_pos, w, bar_height);
        cairo_fill(cr);

        // Draw Border
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x, y_pos, w, bar_height);
        cairo_stroke(cr);

        // Draw PID text
        char pid_str[10];
        sprintf(pid_str, "P%d", timeline[i].pid);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 12);
        cairo_move_to(cr, x + 5, y_pos + 25);
        cairo_set_source_rgb(cr, 1, 1, 1); // White text
        cairo_show_text(cr, pid_str);

        event_count++;
    }

    // Draw Ruler (Time markers)
    cairo_set_source_rgb(cr, 0, 0, 0);
    for (int t = 0; t <= total_time; t += 5) {
        double x = 10 + (t * scale);
        cairo_move_to(cr, x, y_pos + bar_height + 5);
        cairo_line_to(cr, x, y_pos + bar_height + 15);
        cairo_stroke(cr);

        char num[10];
        sprintf(num, "%d", t);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, x - 5, y_pos + bar_height + 25);
        cairo_show_text(cr, num);
    }

    return FALSE;
}

// --- Button: Run Simulation ---
void on_run_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    fetch_data_from_gui();

    // Clear timeline
    memset(timeline, 0, sizeof(timeline));

    // Get Selected Algorithm
    int algo_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_algorithm));

    // Run the Algorithm
    if (algo_idx == 0) schedule_fifo(processes, num_processes, timeline);
    else if (algo_idx == 1) schedule_sjf(processes, num_processes, timeline);
    else if (algo_idx == 2) schedule_stcf(processes, num_processes, timeline);
    else if (algo_idx == 3) schedule_rr(processes, num_processes, 3, timeline); // Hardcoded Q=3 for demo
    else if (algo_idx == 4) {
        mlfq_config_t cfg = {3, (int[]){2,4,8}, 10}; // 3 Queues, Q=2,4,8, Boost=10
        schedule_mlfq(processes, num_processes, &cfg, timeline);
    }

    // Find max time for scaling
    total_time = 0;
    for(int i=0; i<num_processes; i++) {
        if(processes[i].completion_time > total_time) total_time = processes[i].completion_time;
    }

    // Calculate Metrics
    metrics_t m;
    calculate_metrics(processes, num_processes, total_time, &m);

    // Update Label
    char result_txt[512];
    sprintf(result_txt,
        "<b>Metrics Result:</b>\n"
        "Avg Turnaround: %.2f | Avg Waiting: %.2f\n"
        "Avg Response: %.2f | CPU Util: %.1f%%",
        m.avg_turnaround_time, m.avg_waiting_time,
        m.avg_response_time, m.cpu_utilization);
    gtk_label_set_markup(GTK_LABEL(label_metrics), result_txt);

    // Redraw Gantt Chart
    gtk_widget_queue_draw(drawing_area);
}

// --- Boilerplate Setup ---
void on_add_process_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    GtkListStore *store = GTK_LIST_STORE(process_list_store);
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, num_processes+1, 1, 0, 2, 5, 3, 1, -1);
}

void load_default_data() {
    GtkListStore *store = GTK_LIST_STORE(process_list_store);
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter); gtk_list_store_set(store, &iter, 0, 1, 1, 0, 2, 5, 3, 1, -1);
    gtk_list_store_append(store, &iter); gtk_list_store_set(store, &iter, 0, 2, 1, 1, 2, 3, 3, 2, -1);
    gtk_list_store_append(store, &iter); gtk_list_store_set(store, &iter, 0, 3, 1, 2, 2, 8, 3, 1, -1);
}

GtkWidget* create_process_view() {
    process_list_store = (GtkWidget*)gtk_list_store_new(4, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(process_list_store));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "editable", TRUE, NULL); // Note: editing needs a callback to actually save changes
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "PID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Arrival", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Burst", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Priority", renderer, "text", 3, NULL);
    return view;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CPU Scheduler Simulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox_main);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Top: Controls
    GtkWidget *hbox_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox_main), hbox_top, FALSE, FALSE, 0);

    combo_algorithm = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algorithm), "FIFO");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algorithm), "SJF");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algorithm), "STCF");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algorithm), "Round Robin");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algorithm), "MLFQ");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_algorithm), 0);
    gtk_box_pack_start(GTK_BOX(hbox_top), combo_algorithm, FALSE, FALSE, 0);

    GtkWidget *btn_run = gtk_button_new_with_label("Run Simulation");
    g_signal_connect(btn_run, "clicked", G_CALLBACK(on_run_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_top), btn_run, FALSE, FALSE, 0);

    GtkWidget *btn_add = gtk_button_new_with_label("Add Process");
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_process_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_top), btn_add, FALSE, FALSE, 0);

    // Middle: Table
    GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll_win, -1, 200);
    gtk_box_pack_start(GTK_BOX(vbox_main), scroll_win, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(scroll_win), create_process_view());
    load_default_data();

    // Metrics Label
    label_metrics = gtk_label_new("Click Run to see metrics");
    gtk_box_pack_start(GTK_BOX(vbox_main), label_metrics, FALSE, FALSE, 0);

    // Bottom: Gantt Chart
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 300);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), NULL);
    gtk_box_pack_start(GTK_BOX(vbox_main), drawing_area, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}