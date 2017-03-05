#include <gtk/gtk.h>
#include <time.h>
#include <string>
#include <thread>
#include "Sources/measurements_main.h"
#include "send_demon_main.h"
#include "updater_demon_main.h"
#include <memory>		// Smart Pointers

GtkWidget *treeShowView;
GtkWidget *calendar;
GtkWidget *maxSpinner;

sqlite3 *dbMain;


GtkWidget *init_window_params() {
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (window), "InnoMetrics");
    gtk_window_set_default_size(GTK_WINDOW (window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER (window), 0);
    gtk_widget_set_size_request(window, 300, 300);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */
    return window;
}

//region < View And Model  INIT >


static GtkTreeModel *
create_and_fill_model(void) {
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(NUM_COLS,
                               G_TYPE_INT,
                               G_TYPE_UINT64,
                               G_TYPE_UINT64,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_INT,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_BOOLEAN);

    return GTK_TREE_MODEL (store);
}

static void add_column_to_view(const char *title, int colNumber) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, title);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeShowView), col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer, "text", colNumber);
//    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1, "ID", renderer, "text", COL_ID, NULL);
    gtk_tree_view_column_set_resizable(col, TRUE);
    gtk_tree_view_column_set_expand(col, TRUE);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
}

static void //GtkWidget *
create_view_and_model(void) {
    GtkTreeModel *model;

    treeShowView = gtk_tree_view_new();

    /* --- Column *ID* --- */
    add_column_to_view("ID", COL_ID);

    /* --- Column *Focused window* --- */
    add_column_to_view("Focused window", COL_FOCUSED_WINDOW);

    /* --- Column *Client window* --- */
    add_column_to_view("Client window", COL_CLIENT_WINDOW);

    /* --- Column *Window title* --- */
    add_column_to_view("Window title", COL_WINDOW_TITLE);

    /* --- Column *Resource window* --- */
    add_column_to_view("Resource window", COL_RESOURCE_WINDOW);

    /* --- Column *Resource class* --- */
    add_column_to_view("Resource class", COL_RESOURCE_CLASS);

    /* --- Column *Path* --- */
    add_column_to_view("Path", COL_PATH);

    /* --- Column *PID* --- */
    add_column_to_view("PID", COL_PID);

    /* --- Column *Connect time* --- */
    add_column_to_view("Connect time", COL_CONNECT_TIME);

    /* --- Column *Disconnect time* --- */
    add_column_to_view("Disconnect time", COL_DISCONNECT_TIME);

    /* --- Column *Sent* --- */
    add_column_to_view("Sent", COL_SENT);

    model = create_and_fill_model();

    gtk_tree_view_set_model(GTK_TREE_VIEW (treeShowView), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed */

    g_object_unref(model);

    return;
}

//endregion < View And Model  INIT >

//region < Show methods >
bool onlyNonSent = false;

#define SHOW_CONNECT_TIME_BEGIN 1
#define SHOW_CONNECT_TIME_END (SHOW_CONNECT_TIME_BEGIN + 1)
#define SHOW_ALL_ROWS (SHOW_CONNECT_TIME_END + 1)
#define SHOW_LIMIT (SHOW_ALL_ROWS + 1)

static void
show_measuremetns(GtkWidget *widget,
                  gpointer data) {

    sqlite3_stmt *stmt;
    GtkListStore *store;
    GtkTreeIter iter;

    int rc;
//    int rc = sqlite3_open_v2(SQLITE_PATH, &dbMain, SQLITE_OPEN_READONLY, nullptr);
//    if (rc != SQLITE_OK) {
//        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(dbMain));
//        sqlite3_close(dbMain);
//        return;
//    }

    store = gtk_list_store_new(NUM_COLS,
                               G_TYPE_INT,
                               G_TYPE_UINT64,
                               G_TYPE_UINT64,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_INT,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_BOOLEAN);

    rc = sqlite3_prepare_v2(dbMain, DbQueries::SelectAllMetrics().c_str(), -1, &stmt, 0);

    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);


    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 0;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    rawtime = mktime(timeinfo);

    auto tmp = std::to_string(static_cast<long int> (rawtime)) + "000";
    auto t1 = tmp.c_str();

    timeinfo->tm_hour = 23;
    timeinfo->tm_min = 59;
    timeinfo->tm_sec = 59;

    rawtime = mktime(timeinfo);

    auto tmp2 = std::to_string(static_cast<long int> (rawtime)) + "999";
    auto t2 = tmp2.c_str();

    sqlite3_bind_text(stmt, SHOW_CONNECT_TIME_BEGIN, t1, strlen(t1), SQLITE_STATIC);
    sqlite3_bind_text(stmt, SHOW_CONNECT_TIME_END, t2, strlen(t2), SQLITE_STATIC);

    if (onlyNonSent)
        sqlite3_bind_int(stmt, SHOW_ALL_ROWS, 0);
    else
        sqlite3_bind_int(stmt, SHOW_ALL_ROWS, 1);

    int max = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(maxSpinner));
    sqlite3_bind_int(stmt, SHOW_LIMIT, max);

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        time_t rawtime;
        struct tm *timeinfo;
        char buffer1[80];
        char buffer2[80];


        rawtime = sqlite3_column_int64(stmt, COL_CONNECT_TIME) / 1000L;
//        printf("%s,", std::to_string(sqlite3_column_int64(res, COL_CONNECT_TIME)).c_str());

        timeinfo = localtime(&rawtime);
        strftime(buffer1, 80, "%c", timeinfo);
        //delete timeinfo;

        rawtime = sqlite3_column_int64(stmt, COL_DISCONNECT_TIME) / 1000L;
//        printf("%s,", std::to_string(sqlite3_column_int64(res, COL_DISCONNECT_TIME)).c_str());

        timeinfo = localtime(&rawtime);
        strftime(buffer2, 80, "%c", timeinfo);
        //delete timeinfo;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_ID, sqlite3_column_int(stmt, COL_ID),
                           COL_FOCUSED_WINDOW, static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_FOCUSED_WINDOW)),
                           COL_CLIENT_WINDOW, static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_CLIENT_WINDOW)),
                           COL_WINDOW_TITLE, std::string(
                        reinterpret_cast<const char *>(sqlite3_column_text(stmt, COL_WINDOW_TITLE))).c_str(),
                           COL_RESOURCE_WINDOW, std::string(
                        reinterpret_cast<const char *>(sqlite3_column_text(stmt, COL_RESOURCE_WINDOW))).c_str(),
                           COL_RESOURCE_CLASS, std::string(
                        reinterpret_cast<const char *>(sqlite3_column_text(stmt, COL_RESOURCE_CLASS))).c_str(),
                           COL_PATH,
                           std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, COL_PATH))).c_str(),
                           COL_PID, sqlite3_column_int(stmt, COL_PID),
                           COL_CONNECT_TIME, buffer1,
                           COL_DISCONNECT_TIME, buffer2,
                           COL_SENT, sqlite3_column_int(stmt, COL_SENT) == 0 ? false : true,
                           -1);


//        cout <<std::string(reinterpret_cast<const char*>(sqlite3_column_text(res, COL_WINDOW_TITLE))) << "\n";
    }
    auto model = GTK_TREE_MODEL (store);

    gtk_tree_view_set_model(GTK_TREE_VIEW (treeShowView), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed */

    g_object_unref(model);

    if (stmt)
        sqlite3_finalize(stmt);

//    sqlite3_close(dbMeasurements);
    g_print("Done!\n");
}

static void create_max_spin(GtkWidget *grid, int &yCounter) {
    GtkWidget *label = gtk_label_new_with_mnemonic("_Maximum rows");

    GtkAdjustment *adj;
    adj = (GtkAdjustment *) gtk_adjustment_new(30, 5, 500, 5.0, 10.0, 0.0);
    maxSpinner = gtk_spin_button_new(adj, 0, 0);
    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON (maxSpinner), TRUE);

    gtk_grid_attach(GTK_GRID (grid), label, 0, yCounter++, 1, 1);
    gtk_grid_attach(GTK_GRID (grid), maxSpinner, 0, yCounter++, 1, 1);

    gtk_label_set_mnemonic_widget(GTK_LABEL (label), maxSpinner);
}

static void create_calendar(GtkWidget *grid, int &yCounter) {
    GtkWidget *label = gtk_label_new_with_mnemonic("_Pick date");

    calendar = gtk_calendar_new();

    gtk_grid_attach(GTK_GRID (grid), label, 0, yCounter++, 1, 1);
    gtk_grid_attach(GTK_GRID (grid), calendar, 0, yCounter++, 1, 1);

    gtk_label_set_mnemonic_widget(GTK_LABEL (label), calendar);

}

static void create_show_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height) {
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *showButton;

    int yCounter = 0;

    ownGrid = gtk_grid_new();

    frame = gtk_frame_new("Show");


    create_calendar(ownGrid, yCounter);

    create_max_spin(ownGrid, yCounter);

    showButton = gtk_button_new_with_label("Show info");
    g_signal_connect (showButton, "clicked", G_CALLBACK(show_measuremetns), NULL);

    gtk_grid_attach(GTK_GRID (ownGrid), showButton, 0, yCounter++, 1, 1);

    gtk_widget_set_hexpand(showButton, TRUE);
    gtk_widget_set_hexpand(ownGrid, FALSE);

    gtk_container_add(GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach(GTK_GRID (outsideGrid), frame, left, top, width, height);

}

//endregion < Show methods >

static void
update_program(GtkWidget *widget,
              gpointer data) {
    update();
    exit(0);
}

static void
refresh_table(GtkWidget *widget,
              gpointer data) {

}

static void
clean_table(GtkWidget *widget,
            gpointer data) {

}

static void create_refresh(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height) {
    GtkWidget *ownGrid;

    GtkWidget *updateButton;
    GtkWidget *refreshButton;
    GtkWidget *cleanButton;

    ownGrid = gtk_grid_new();

    updateButton = gtk_button_new_with_label("Update InnoMetrics");
    g_signal_connect (updateButton, "clicked", G_CALLBACK(update_program), NULL);

    gtk_grid_attach(GTK_GRID (ownGrid), updateButton, 0, 0, 2, 1);


    if(!check_version()){
        gtk_widget_set_sensitive(updateButton, FALSE);
    }



    refreshButton = gtk_button_new_with_label("Refresh");
    g_signal_connect (refreshButton, "clicked", G_CALLBACK(refresh_table), NULL);

    gtk_grid_attach(GTK_GRID (ownGrid), refreshButton, 0, 1, 1, 1);

    cleanButton = gtk_button_new_with_label("Clean DB");
    g_signal_connect (refreshButton, "clicked", G_CALLBACK(clean_table), NULL);

    gtk_widget_set_hexpand(refreshButton, TRUE);
    gtk_widget_set_hexpand(cleanButton, TRUE);
    gtk_widget_set_hexpand(ownGrid, FALSE);

    gtk_grid_attach(GTK_GRID (ownGrid), cleanButton, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID (outsideGrid), ownGrid, left, top, width, height);

}

static void
expander_callback(GObject *object,
                  GParamSpec *param_spec,
                  gpointer user_data) {
    GtkExpander *expander;

    expander = GTK_EXPANDER (object);

    if (gtk_expander_get_expanded(expander)) {
        // Show or create widgets
    } else {
        // Hide or destroy widgets
    }
}

//region < Collecting methods >

static gboolean continue_collecting_timer = FALSE;
static gboolean start_collecting_timer = FALSE;
static int sec_collecting_expired = 0;


static gboolean
_collecting_timer_label_update(gpointer data) {
    GtkLabel *label = (GtkLabel *) data;
    char buf[256];
    memset(&buf, 0x0, 256);
    snprintf(buf, 255, "Time elapsed: %d secs", ++sec_collecting_expired);
    gtk_label_set_label(label, buf);
    return continue_collecting_timer;
}


static void
start_measurments(GtkWidget *button,
                  gpointer data) {
//    static pid_t pid = -1;
    static pthread_t measuringTask;
    (void) button;/*Avoid compiler warnings*/


    GtkWidget **labels = (GtkWidget **) data;
    if (!start_collecting_timer) {
        g_timeout_add_seconds(1, _collecting_timer_label_update, labels[0]);
        gtk_label_set_label((GtkLabel *) labels[1], "Status: Launched");
        gtk_button_set_label((GtkButton *)button, "Stop measuring");
        start_collecting_timer = TRUE;
        continue_collecting_timer = TRUE;
//        pid = fork();
//        if (pid == 0) {
//            measurements_main();
//            g_print("Child Exit!\n");
//            exit(0);
//        }
        pthread_create (&measuringTask, NULL, measurements_main, NULL);
    } else {
        gtk_label_set_label((GtkLabel *) labels[1], "Status: Idle");
        gtk_button_set_label((GtkButton *)button, "Start measuring");
//        if (pid != -1) {
//            kill(pid, SIGKILL);
//            pid = -1;
//        }

        pthread_cancel(measuringTask);

        pthread_join(measuringTask, NULL);

        sec_collecting_expired--;
        start_collecting_timer = FALSE;
        continue_collecting_timer = FALSE;
    };

}

static void
create_start_collecting_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height) {
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *startButton;
    int yCounter = 0;

    GtkWidget **timerStatusLabels = new GtkWidget *[2];

    timerStatusLabels[0] = gtk_label_new("Time elapsed: 0 secs");
    timerStatusLabels[1] = gtk_label_new("Status: Idle");

    ownGrid = gtk_grid_new();
    frame = gtk_frame_new("Process of measurements");


    gtk_grid_attach(GTK_GRID (ownGrid), timerStatusLabels[1], 0, yCounter++, 1, 1);
    gtk_grid_attach(GTK_GRID (ownGrid), timerStatusLabels[0], 0, yCounter++, 1, 1);


    startButton = gtk_button_new_with_label("Start Measuring");
    g_signal_connect (startButton, "clicked", G_CALLBACK(start_measurments), (gpointer) timerStatusLabels);

    gtk_grid_attach(GTK_GRID (ownGrid), startButton, 0, yCounter++, 1, 1);

    gtk_widget_set_hexpand(startButton, TRUE);
    gtk_widget_set_hexpand(ownGrid, FALSE);

    gtk_container_add(GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach(GTK_GRID (outsideGrid), frame, left, top, width, height);

    continue_collecting_timer = FALSE;
    start_collecting_timer = FALSE;

}


//static void create_filter_expander(GtkWidget *outsideGrid, int &yCounter) {
//
//    GtkWidget *expander = gtk_expander_new_with_mnemonic("_More Options");
//    g_signal_connect (expander, "notify::expanded",
//                      G_CALLBACK(expander_callback), NULL);
//
//
//    GtkListStore *listStore;
//    GtkTreeIter iter;
//    GtkWidget *view;
//    GtkCellRenderer *renderer;
//
//    view = gtk_tree_view_new();
//    renderer = gtk_cell_renderer_text_new();
//    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (view), -1, "Name", renderer, "text", 0, NULL);
//    g_object_set(renderer, "editable", TRUE, NULL);
//    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
//
//    gtk_widget_set_size_request(view, -1, 150);
//
//    listStore = gtk_list_store_new(1, G_TYPE_STRING);
//    // Add a new row to the model
//    gtk_list_store_append(listStore, &iter);
//    gtk_list_store_set(listStore, &iter, 0, "row", -1);
//
//    gtk_tree_view_set_model(GTK_TREE_VIEW (view), GTK_TREE_MODEL(listStore));
//
//    gtk_container_add(GTK_CONTAINER(expander), view);
//    gtk_grid_attach(GTK_GRID (outsideGrid), expander, 0, yCounter++, 1, 1);
//}

//endregion

//region < Send methods >

static gboolean continue_send_timer = FALSE;
static gboolean start_send_timer = FALSE;
static int sec_send_expired = 0;


static gboolean
_send_timer_label_update(gpointer data) {
    GtkLabel *label = (GtkLabel *) data;
    char buf[256];
    memset(&buf, 0x0, 256);
    snprintf(buf, 255, "Time elapsed: %d secs", ++sec_send_expired);
    gtk_label_set_label(label, buf);
    return continue_send_timer;
}


int pthread_state = 0;
static void
start_send(GtkWidget *button,
           gpointer data) {
//    static pid_t pid = -1;
    static pthread_t sendTask;
    (void) button;/*Avoid compiler warnings*/

    GtkWidget **labels = (GtkWidget **) data;
    if (!start_send_timer) {
        g_timeout_add_seconds(1, _send_timer_label_update, labels[0]);
        gtk_label_set_label((GtkLabel *) labels[1], "Status: Launched");
        gtk_button_set_label((GtkButton *)button, "Stop sending");
        start_send_timer = TRUE;
        continue_send_timer = TRUE;
//        pid = fork();
//        if (pid == 0) {
//            //send_demon_main();
//            g_print("Child Exit!\n");
//            exit(0);
//        }
        pthread_state = 0;
        pthread_create (&sendTask, NULL, send_demon_main, &pthread_state);
    } else {
        gtk_label_set_label((GtkLabel *) labels[1], "Status: Idle");
        gtk_button_set_label((GtkButton *)button, "Start sending");


//        if (pid != -1) {
//            kill(pid, SIGKILL);
//            pid = -1;
//        }

        pthread_state = -1;

        pthread_join(sendTask, NULL);

        sec_send_expired--;
        start_send_timer = FALSE;
        continue_send_timer = FALSE;
    };
}

static void
send_rows(GtkWidget *button,
          gpointer data) {
    static pid_t pid = -1;
    (void) button;/*Avoid compiler warnings*/

    pid = fork();
    if (pid == 0) {
        send_data();
        g_print("Send rows done!\n");
        exit(0);
    }
}

static void
create_start_sending_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height) {
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *startSendButton;
    GtkWidget *sendButton;
    GtkWidget **timerStatusLabels = new GtkWidget *[2];

    int yCounter = 0;

    timerStatusLabels[0] = gtk_label_new("Time elapsed: 0 secs");
    timerStatusLabels[1] = gtk_label_new("Status: Idle");

    ownGrid = gtk_grid_new();


    frame = gtk_frame_new("Process of sending");


    gtk_grid_attach(GTK_GRID (ownGrid), timerStatusLabels[1], 0, yCounter++, 1, 1);
    gtk_grid_attach(GTK_GRID (ownGrid), timerStatusLabels[0], 0, yCounter++, 1, 1);


    //region SendButton
    sendButton = gtk_button_new_with_label("Send 20 rows");
    g_signal_connect (sendButton, "clicked", G_CALLBACK(send_rows), (gpointer) timerStatusLabels);

    gtk_widget_set_hexpand(sendButton, TRUE);

    gtk_grid_attach(GTK_GRID (ownGrid), sendButton, 0, yCounter++, 1, 1);
    //endregion SendButton

    //region StartSendButton
    startSendButton = gtk_button_new_with_label("Start process");
    g_signal_connect (startSendButton, "clicked", G_CALLBACK(start_send), (gpointer) timerStatusLabels);

    gtk_widget_set_hexpand(startSendButton, TRUE);

    gtk_grid_attach(GTK_GRID (ownGrid), startSendButton, 0, yCounter++, 1, 1);
    //endregion StartSendButton




    gtk_widget_set_hexpand(ownGrid, FALSE);

    gtk_container_add(GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach(GTK_GRID (outsideGrid), frame, left, top, width, height);

    continue_send_timer = FALSE;
    start_send_timer = FALSE;
}

//endregion

//region < View + Buttons >

GtkWidget *onlyNonSentButton;
GtkWidget *allButton;
bool manual = false;

static void set_toggled_to_table_buttons_all(GtkWidget *widget, gpointer data) {
    if (manual)
        return;
    gtk_widget_set_sensitive(widget, FALSE);
    onlyNonSent = false;

    manual = true;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(onlyNonSentButton), FALSE);
    gtk_widget_set_sensitive(onlyNonSentButton, TRUE);
    manual = false;

}

static void set_toggled_to_table_buttons_only(GtkWidget *widget, gpointer data) {
    if (manual)
        return;
    gtk_widget_set_sensitive(widget, FALSE);
    onlyNonSent = true;

    manual = true;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(allButton), FALSE);
    gtk_widget_set_sensitive(allButton, TRUE);
    manual = false;
}

static GtkWidget *create_view_boxes() { //GtkWidget *outsideGrid, gint left, gint top, gint width, gint height
    GtkWidget *scrolled_window;
    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER (scrolled_window), 1);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    create_view_and_model();

    gtk_widget_set_hexpand(treeShowView, TRUE);
    gtk_widget_set_vexpand(treeShowView, TRUE);

    gtk_container_add(GTK_CONTAINER (scrolled_window), treeShowView);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);

    gtk_button_box_set_layout(GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_CENTER);
    gtk_box_set_spacing(GTK_BOX (bbox), 1);


    onlyNonSentButton = gtk_toggle_button_new_with_label("Only not sent");
    allButton = gtk_toggle_button_new_with_label("All metrics");

    g_signal_connect (onlyNonSentButton, "toggled", G_CALLBACK(set_toggled_to_table_buttons_only), NULL);
    g_signal_connect (allButton, "toggled", G_CALLBACK(set_toggled_to_table_buttons_all), NULL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(onlyNonSentButton), TRUE);

    gtk_container_add(GTK_CONTAINER (bbox), onlyNonSentButton);
    gtk_container_add(GTK_CONTAINER (bbox), allButton);

    gtk_box_pack_start(GTK_BOX(box), bbox, FALSE, FALSE, 2);
    gtk_box_pack_end(GTK_BOX(box), scrolled_window, TRUE, TRUE, 2);

//    gtk_grid_attach (GTK_GRID (outsideGrid), box, left, top, width, height);
    return box;
}
//endregion < View + Buttons >

//region < Filtering >
GtkListStore *listStore;
GtkWidget *treeFilterView;

static void
add_filter(GtkWidget *button,
                  gpointer data) {
    (void) button;/*Avoid compiler warnings*/

    static const char* newFilter = "New filter";

    GtkTreeIter iter;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare(dbMain, DbQueries::InsertTitleFilter().c_str(), -1, &stmt, 0);

    sqlite3_bind_text(stmt, 1, newFilter, strlen(newFilter), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if(rc == SQLITE_DONE)
    {
        int id = sqlite3_last_insert_rowid(dbMain);
        gtk_list_store_append(listStore, &iter);
        gtk_list_store_set(listStore, &iter, 0, id, 1, "New Filter", -1);
    }

}

static void
remove_filter(GtkWidget *button,
           gpointer data) {
    (void) button;/*Avoid compiler warnings*/

    GtkTreeIter iter;

    auto selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeFilterView));

    if(gtk_tree_selection_get_selected(selection, NULL, &iter)){

        gint id;
        gtk_tree_model_get(GTK_TREE_MODEL (listStore), &iter, 0 , &id, -1 );

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare(dbMain, DbQueries::DeleteTitleFilter().c_str(), -1, &stmt, 0);

        sqlite3_bind_int(stmt, 1, id);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if(rc == SQLITE_DONE)
        {
            gtk_list_store_remove (listStore, &iter);
        }
    }

}



void filter_edit_callback(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
    GtkTreeIter iter;
    GtkTreePath *path;
    path = gtk_tree_path_new_from_string (path_string);
    gtk_tree_model_get_iter (GTK_TREE_MODEL (listStore), &iter, path);
    gtk_tree_path_free (path);


    gint id;
    gtk_tree_model_get(GTK_TREE_MODEL (listStore), &iter, 0 , &id, -1 );

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare(dbMain, DbQueries::UpdateTitleFilter().c_str(), -1, &stmt, 0);

    sqlite3_bind_text(stmt, 1, new_text, strlen(new_text), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if(rc == SQLITE_DONE) {
        gtk_list_store_set (listStore, &iter, 1, new_text, -1);
    }
}

static void fill_rows(){
    sqlite3_stmt *stmt;
    GtkTreeIter iter;
    // Add a new row to the model
//    gtk_list_store_append(listStore, &iter);
//    gtk_list_store_set(listStore, &iter, 0, "row", -1);
    int rc;

    rc = sqlite3_prepare_v2(dbMain, DbQueries::SelectTitleFilter().c_str(), -1, &stmt, 0);

    rc = sqlite3_step(stmt);

    while (rc == SQLITE_ROW) {
        gtk_list_store_append(listStore, &iter);
        gtk_list_store_set(listStore, &iter, 0, sqlite3_column_int(stmt, 0),1 , std::string(
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))).c_str(), -1);

        rc = sqlite3_step(stmt);
    }

//    g_object_unref(model);

    if (stmt)
        sqlite3_finalize(stmt);
}

static void create_filtering(GtkWidget *outsideGrid, gint left, gint top, gint width, gint height) {

    GtkWidget *scrolled_window;

    GtkWidget *frame;


    GtkCellRenderer *renderer;

    GtkWidget *addButton;
    GtkWidget *removeButton;
    GtkWidget *ownGrid;

    int yCounter = 0;

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER (scrolled_window), 1);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_set_size_request(scrolled_window, -1, 150);

    frame = gtk_frame_new("Filtering");
    ownGrid = gtk_grid_new();

    treeFilterView = gtk_tree_view_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (treeFilterView), -1, "Id", renderer, "text", 0, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (treeFilterView), -1, "Name", renderer, "text", 1, NULL);

    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect(renderer, "edited", (GCallback) filter_edit_callback, NULL);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeFilterView), FALSE);


    gtk_widget_set_hexpand(treeFilterView, TRUE);

    listStore = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);

    fill_rows();


    gtk_tree_view_set_model(GTK_TREE_VIEW (treeFilterView), GTK_TREE_MODEL(listStore));


    addButton = gtk_button_new_with_label("+");
    g_signal_connect (addButton, "clicked", G_CALLBACK(add_filter), NULL);


    removeButton = gtk_button_new_with_label("-");
    g_signal_connect (removeButton, "clicked", G_CALLBACK(remove_filter), NULL);


    GtkWidget *bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_EXPAND);

    gtk_container_add(GTK_CONTAINER (bbox), addButton);
    gtk_container_add(GTK_CONTAINER (bbox), removeButton);

    gtk_container_add(GTK_CONTAINER (scrolled_window), treeFilterView);

    gtk_grid_attach(GTK_GRID (ownGrid), scrolled_window, 0, yCounter++, 1, 1);
    gtk_grid_attach(GTK_GRID (ownGrid), bbox, 0, yCounter++, 1, 1);

    gtk_container_add(GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach(GTK_GRID (outsideGrid), frame, left, top, width, height);
}

//endregion


void HandleInitialization() {
    sqlite3_stmt *stmt;

    int  rc = sqlite3_prepare(dbMain, DbQueries::CreateTitleFilterTable().c_str(), -1, &stmt, 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Filter error:\n");
    } else {
        fprintf(stdout, "Table Filter created successfully\n");
    }

    rc = sqlite3_prepare(dbMain, DbQueries::CreateMetricTable().c_str(), -1, &stmt, 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Metric error:\n");
    } else {
        fprintf(stdout, "Table Metric created successfully\n");
    }
}

int
main(int argc, char **argv) {
    static GtkWidget *window;

    int rc = sqlite3_open(SQLITE_PATH, &dbMain);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(dbMain));
        sqlite3_close(dbMain);
        return 0;
    }

    HandleInitialization();

    GtkWidget *grid2;
    int grid2VC = 0;


    gtk_init(&argc, &argv);
    window = init_window_params();


    grid2 = gtk_grid_new();
    gtk_widget_set_halign(grid2, GTK_ALIGN_FILL);
    gtk_widget_set_valign(grid2, GTK_ALIGN_FILL);

    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_halign(hpaned, GTK_ALIGN_FILL);
    gtk_widget_set_valign(hpaned, GTK_ALIGN_FILL);
    gtk_container_add(GTK_CONTAINER (window), hpaned);
    gtk_widget_show(hpaned);

    gtk_paned_pack1(GTK_PANED (hpaned), grid2, FALSE, FALSE);

    gtk_paned_pack2(GTK_PANED (hpaned), create_view_boxes(), TRUE, FALSE);




    create_refresh(window, grid2, 0, grid2VC++, 1, 1);
    create_show_panel(window, grid2, 0, grid2VC++, 1, 1);
    create_start_collecting_panel(window, grid2, 0, grid2VC++, 1, 1);
    create_start_sending_panel(window, grid2, 0, grid2VC++, 1, 1);
    create_filtering(grid2, 0, grid2VC++, 1, 1);


    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
