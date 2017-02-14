#include <gtk/gtk.h>
#include <time.h>
#include <string>
#include <thread>
#include "Sources/measurements_main.h"
#include "send_demon_main.h"
#include <memory>		// Smart Pointers

GtkWidget *view;
GtkWidget *calendar;
GtkWidget *maxSpinner;


static GtkTreeModel *
create_and_fill_model (void)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new (NUM_COLS,
                                G_TYPE_INT,
                                G_TYPE_UINT64,
                                G_TYPE_UINT64,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_INT,
                                G_TYPE_STRING,
                                G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

static void add_column_to_view(const char* title, int colNumber){
    GtkCellRenderer     *renderer;
    GtkTreeViewColumn   *col;
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, title);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer, "text", colNumber);
//    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1, "ID", renderer, "text", COL_ID, NULL);
    gtk_tree_view_column_set_resizable(col, TRUE);
    gtk_tree_view_column_set_expand(col, TRUE);
    gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_FIXED);
}

static void //GtkWidget *
create_view_and_model (void)
{
    GtkTreeModel        *model;

    view = gtk_tree_view_new ();

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

    model = create_and_fill_model ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    return;
}

GtkWidget * init_window_params(){
    GtkWidget *window;
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "InnoMetrics");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_widget_set_size_request (window, 300, 300);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */
    return window;
}

#define SQLITE_PATH "metrics.db"

static void
show_measuremetns (GtkWidget *widget,
                    gpointer   data)
{
    static sqlite3 *db;
    sqlite3_stmt *stmt;
    GtkListStore  *store;
    GtkTreeIter    iter;

    int rc = sqlite3_open_v2(SQLITE_PATH, &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    store = gtk_list_store_new (NUM_COLS,
                                G_TYPE_INT,
                                G_TYPE_UINT64,
                                G_TYPE_UINT64,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_INT,
                                G_TYPE_STRING,
                                G_TYPE_STRING);

    rc = sqlite3_prepare_v2(db, DbQueries::SelectAllMetrics().c_str(), -1, &stmt, 0);

    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 0;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_mon = 0;
    timeinfo->tm_sec = 0;

    rawtime = mktime(timeinfo);

    auto tmp =std::to_string(static_cast<long int> (rawtime)) + "000";
    auto t1 =  tmp.c_str();

    timeinfo->tm_hour = 23;
    timeinfo->tm_min = 59;
    timeinfo->tm_sec = 59;

    rawtime = mktime(timeinfo);

    auto tmp2 = std::to_string(static_cast<long int> (rawtime)) + "999";
    auto t2 = tmp2.c_str();

    sqlite3_bind_text(stmt, 1, t1, strlen(t1), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, t2, strlen(t2), SQLITE_STATIC);

    int max = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(maxSpinner));
    sqlite3_bind_int(stmt, 3, max);

    while (sqlite3_step(stmt) == SQLITE_ROW){

        time_t rawtime;
        struct tm * timeinfo;
        char buffer1 [80];
        char buffer2 [80];


        rawtime = sqlite3_column_int64(stmt, COL_CONNECT_TIME) / 1000L;
//        printf("%s,", std::to_string(sqlite3_column_int64(res, COL_CONNECT_TIME)).c_str());

        timeinfo = localtime (&rawtime);
        strftime (buffer1,80,"%c",timeinfo);
        //delete timeinfo;

        rawtime = sqlite3_column_int64(stmt, COL_DISCONNECT_TIME) / 1000L;
//        printf("%s,", std::to_string(sqlite3_column_int64(res, COL_DISCONNECT_TIME)).c_str());

        timeinfo = localtime (&rawtime);
        strftime (buffer2,80,"%c",timeinfo);
        //delete timeinfo;

        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            COL_ID, sqlite3_column_int(stmt, COL_ID),
                            COL_FOCUSED_WINDOW, static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_FOCUSED_WINDOW)),
                            COL_CLIENT_WINDOW, static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_CLIENT_WINDOW)),
                            COL_WINDOW_TITLE, std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_WINDOW_TITLE))).c_str(),
                            COL_RESOURCE_WINDOW, std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_RESOURCE_WINDOW))).c_str(),
                            COL_RESOURCE_CLASS, std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_RESOURCE_CLASS))).c_str(),
                            COL_PATH, std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_PATH))).c_str(),
                            COL_PID, sqlite3_column_int(stmt, COL_PID),
                            COL_CONNECT_TIME, buffer1,
                            COL_DISCONNECT_TIME, buffer2,
                            -1);


//        cout <<std::string(reinterpret_cast<const char*>(sqlite3_column_text(res, COL_WINDOW_TITLE))) << "\n";
    }
    auto model = GTK_TREE_MODEL (store);

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    if(stmt)
        sqlite3_finalize(stmt);

    sqlite3_close(db);
    g_print ("Done!\n");
}


static void create_calendar( GtkWidget *window , GtkWidget *grid, gint left, gint top, gint width, gint height)
{

    GtkWidget *frame;

    /* Calendar widget */
    frame = gtk_frame_new ("Pick date");
//    gtk_widget_set_hexpand (frame, TRUE);

    gtk_grid_attach (GTK_GRID (grid), frame, left, top, width, height);

    calendar = gtk_calendar_new ();

    gtk_container_add (GTK_CONTAINER (frame), calendar);

    gtk_widget_show_all (window);
}

static void create_max_spin( GtkWidget *window , GtkWidget *grid, gint left, gint top, gint width, gint height)
{

    GtkWidget *frame;
    GtkAdjustment *adj;

    frame = gtk_frame_new ("Maximum rows");

    gtk_grid_attach (GTK_GRID (grid), frame, left, top, width, height);

    adj = (GtkAdjustment *) gtk_adjustment_new (30, 5, 500, 5.0, 10.0, 0.0);
    maxSpinner = gtk_spin_button_new (adj, 0, 0);
    gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (maxSpinner), TRUE);

    gtk_container_add (GTK_CONTAINER (frame), maxSpinner);
}

static void create_show_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height)
{
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *showButton;

    ownGrid =  gtk_grid_new();

    frame = gtk_frame_new ("Show settings");

    create_calendar(window, ownGrid, 0, 0, 1, 1);
    create_max_spin(window, ownGrid, 0, 1, 1, 1);

    showButton = gtk_button_new_with_label ("Show info");
    g_signal_connect (showButton, "clicked", G_CALLBACK (show_measuremetns), NULL);

    gtk_grid_attach (GTK_GRID (ownGrid), showButton, 0, 2, 1, 1);

    gtk_widget_set_hexpand (showButton, TRUE);
    gtk_widget_set_hexpand (ownGrid, FALSE);

    gtk_container_add (GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach (GTK_GRID (outsideGrid), frame, left, top, width, height);

}

static void
expander_callback (GObject    *object,
                   GParamSpec *param_spec,
                   gpointer    user_data)
{
    GtkExpander *expander;

    expander = GTK_EXPANDER (object);

    if (gtk_expander_get_expanded (expander))
    {
        // Show or create widgets
    }
    else
    {
        // Hide or destroy widgets
    }
}

//region < Collecting methods >

static gboolean continue_collecting_timer = FALSE;
static gboolean start_collecting_timer = FALSE;
static int sec_collecting_expired = 0;


static gboolean
_collecting_timer_label_update(gpointer data)
{
    GtkLabel *label = (GtkLabel*)data;
    char buf[256];
    memset(&buf, 0x0, 256);
    snprintf(buf, 255, "Time elapsed: %d secs", ++sec_collecting_expired);
    gtk_label_set_label(label, buf);
    return continue_collecting_timer;
}

static void
start_measurments(GtkWidget *button,
                  gpointer data)
{
    static pid_t pid = -1;
    (void)button;/*Avoid compiler warnings*/


    GtkWidget **labels = (GtkWidget **)data;
    if(!start_collecting_timer)
    {
        g_timeout_add_seconds(1, _collecting_timer_label_update, labels[0]);
        gtk_label_set_label((GtkLabel*)labels[1], "Status: Launched");
        start_collecting_timer = TRUE;
        continue_collecting_timer = TRUE;
        pid = fork();
        if(pid==0){
            measurements_main();
            g_print ("Child Exit!\n");
            exit(0);
        }
    }else{
        gtk_label_set_label((GtkLabel*)labels[1], "Status: Idle");
        if(pid != -1){
            kill(pid, SIGKILL);
            pid = -1;
        }

        sec_collecting_expired--;
        start_collecting_timer = FALSE;
        continue_collecting_timer = FALSE;
    };

}

static void create_start_collecting_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height){
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *startButton;

    GtkWidget **timerStatusLabels= new GtkWidget*[2];

    timerStatusLabels[0] = gtk_label_new("Time elapsed: 0 secs");
    timerStatusLabels[1] = gtk_label_new("Status: Idle");

    ownGrid =  gtk_grid_new();
    frame = gtk_frame_new ("Process of measurements");


    gtk_grid_attach (GTK_GRID (ownGrid), timerStatusLabels[1], 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (ownGrid), timerStatusLabels[0], 0, 1, 1, 1);


    GtkWidget *expander = gtk_expander_new_with_mnemonic ("_More Options");
    g_signal_connect (expander, "notify::expanded",
                      G_CALLBACK (expander_callback), NULL);

    gtk_grid_attach (GTK_GRID (ownGrid), expander, 0, 2, 1, 1);

    startButton = gtk_button_new_with_label ("Start Measuring");
    g_signal_connect (startButton, "clicked", G_CALLBACK(start_measurments), (gpointer)timerStatusLabels);

    gtk_grid_attach (GTK_GRID (ownGrid), startButton, 0, 3, 1, 1);

    gtk_widget_set_hexpand (startButton, TRUE);
    gtk_widget_set_hexpand (ownGrid, FALSE);

    gtk_container_add (GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach (GTK_GRID (outsideGrid), frame, left, top, width, height);

    continue_collecting_timer = FALSE;
    start_collecting_timer = FALSE;

}

//endregion

//region < Send methods >

static gboolean continue_send_timer = FALSE;
static gboolean start_send_timer = FALSE;
static int sec_send_expired = 0;

static gboolean
_send_timer_label_update(gpointer data)
{
    GtkLabel *label = (GtkLabel*)data;
    char buf[256];
    memset(&buf, 0x0, 256);
    snprintf(buf, 255, "Time elapsed: %d secs", ++sec_send_expired);
    gtk_label_set_label(label, buf);
    return continue_send_timer;
}

static void
start_send (GtkWidget *button,
            gpointer   data)
{
    static pid_t pid = -1;
    (void)button;/*Avoid compiler warnings*/

    GtkWidget **labels = (GtkWidget **)data;
    if(!start_send_timer)
    {
        g_timeout_add_seconds(1, _send_timer_label_update, labels[0]);
        gtk_label_set_label((GtkLabel*)labels[1], "Status: Launched");
        start_send_timer = TRUE;
        continue_send_timer = TRUE;
        pid = fork();
        if(pid==0){
//            send_demon_main();
            g_print ("Child Exit!\n");
            exit(0);
        }
    }else{
        gtk_label_set_label((GtkLabel*)labels[1], "Status: Idle");
        if(pid != -1){
            kill(pid, SIGKILL);
            pid = -1;
        }

        sec_send_expired--;
        start_send_timer = FALSE;
        continue_send_timer = FALSE;
    };
}

static void create_start_sending_panel(GtkWidget *window, GtkWidget *outsideGrid, gint left, gint top, gint width, gint height){
    GtkWidget *frame;
    GtkWidget *ownGrid;
    GtkWidget *startSendButton;
    GtkWidget **timerStatusLabels= new GtkWidget*[2];
    timerStatusLabels[0] = gtk_label_new("Time elapsed: 0 secs");
    timerStatusLabels[1] = gtk_label_new("Status: Idle");

    ownGrid =  gtk_grid_new();


    frame = gtk_frame_new ("Process of sending");


    gtk_grid_attach (GTK_GRID (ownGrid), timerStatusLabels[1], 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (ownGrid), timerStatusLabels[0], 0, 1, 1, 1);

    startSendButton = gtk_button_new_with_label ("Start Sending");
    g_signal_connect (startSendButton, "clicked", G_CALLBACK (start_send), (gpointer)timerStatusLabels);

    gtk_widget_set_hexpand (startSendButton, FALSE);
    gtk_widget_set_vexpand (startSendButton, FALSE);
    gtk_grid_attach (GTK_GRID (ownGrid), startSendButton, 0, 2, 1, 1);

    gtk_widget_set_hexpand (startSendButton, TRUE);
    gtk_widget_set_hexpand (ownGrid, FALSE);

    gtk_container_add (GTK_CONTAINER (frame), ownGrid);

    gtk_grid_attach (GTK_GRID (outsideGrid), frame, left, top, width, height);

    continue_send_timer = FALSE;
    start_send_timer = FALSE;
}

//endregion

int
main123 (int argc, char **argv)
{
    static GtkWidget *window;

    GtkWidget *grid;
    GtkWidget *grid2;
    GtkWidget *scrolled_window;

    GtkWidget *startSendButton;


    gtk_init (&argc, &argv);
    window = init_window_params();


    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    grid =  gtk_grid_new();
    grid2 =  gtk_grid_new();
    gtk_widget_set_halign (grid,  GTK_ALIGN_FILL);
    gtk_widget_set_valign (grid,  GTK_ALIGN_FILL);
    gtk_container_add (GTK_CONTAINER (window), grid);

    create_view_and_model ();

    gtk_widget_set_hexpand (view, TRUE);
    gtk_widget_set_vexpand (view, TRUE);

    gtk_container_add (GTK_CONTAINER (scrolled_window), view);

    gtk_grid_attach (GTK_GRID (grid), scrolled_window, 0, 0, 2, 1);
    gtk_grid_attach (GTK_GRID (grid), grid2, 2, 0, 1, 1);

    create_show_panel(window, grid2, 0, 0, 1, 1);
    create_start_collecting_panel(window, grid2, 0, 1, 1, 1);
    create_start_sending_panel(window, grid2, 0, 2, 1, 1);



    gtk_widget_show_all (window);

    gtk_main ();

    return 0;
}
