//
// Created by alon on 11.03.17.
//

#ifndef INNOMETRICS_AUTHORIZATION_H
#define INNOMETRICS_AUTHORIZATION_H
#include <gtk/gtk.h>
#include "send_demon_main.h"


struct FooLoginDialogPrivate
{
    GtkWidget *username_entry;
    GtkWidget *password_entry;
};

void SaveToken(sqlite3 *db, const char* user_name,const char* token) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare(db, DbQueries::InsertToken().c_str(), -1, &stmt, 0);
    long stamp = getMilliseconds();

    sqlite3_bind_text(stmt, 1, user_name, -1, 0);
    sqlite3_bind_text(stmt, 2, token, -1, 0);
    sqlite3_bind_int64(stmt, 3, stamp);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);


    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error:%s\n", sqlite3_errmsg(db));
    } else {
        //fprintf(stdout, "Insert query complited successfully\n");
    }
}

// Function to open a dialog box with a message
bool
AuthorizationDialog (GtkWindow *parent, gchar *message, sqlite3 *db, char *token)
{
    GtkWidget *dialog, *label, *content_area, *warning_lable;
    GtkDialogFlags flags;

    // Create the widgets
    flags = static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT);
    dialog = gtk_dialog_new_with_buttons (message,
                                          parent,
                                          flags,
                                          ("_OK"),
                                          GTK_RESPONSE_ACCEPT,
                                          ("_Cancel"),
                                          GTK_RESPONSE_REJECT,
                                          NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

    //Content

    struct FooLoginDialogPrivate priv;
    GtkWidget *grid;


    grid = gtk_grid_new ();
    g_object_set (grid, "margin", 6, NULL);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 6);
    gtk_grid_set_row_spacing (GTK_GRID (grid), 4);
    gtk_box_pack_start (GTK_BOX (content_area), grid, TRUE, TRUE, 0);

    warning_lable = gtk_label_new ("");
    gtk_widget_set_halign (warning_lable, GTK_ALIGN_END);
    gtk_grid_attach (GTK_GRID (grid), warning_lable, 0, 0, 2, 1);

    label = gtk_label_new ("User name:");
    gtk_widget_set_halign (label, GTK_ALIGN_END);
    gtk_grid_attach (GTK_GRID (grid), label, 0, 1, 1, 1);
    priv.username_entry = gtk_entry_new ();
    gtk_widget_set_hexpand (priv.username_entry, TRUE);
    gtk_grid_attach (GTK_GRID (grid), priv.username_entry,
                     1, 1, 1, 1);

    label = gtk_label_new ("Password:");
    gtk_widget_set_halign (label, GTK_ALIGN_END);
    gtk_grid_attach (GTK_GRID (grid), label, 0, 2, 1, 1);
    priv.password_entry = gtk_entry_new ();
    gtk_widget_set_hexpand (priv.password_entry, TRUE);
    gtk_entry_set_visibility (GTK_ENTRY (priv.password_entry),
                              FALSE);
    gtk_entry_set_activates_default (GTK_ENTRY (priv.password_entry),
                                     TRUE);
    gtk_grid_attach (GTK_GRID (grid), priv.password_entry,
                     1, 2, 1, 1);


    // Ensure that the dialog box is destroyed when the user responds

//    g_signal_connect_swapped (dialog,
//                              "response",
//                              G_CALLBACK (gtk_widget_destroy),
//                              dialog);


    gtk_widget_show_all (dialog);

    bool success = false;

    gint result = gtk_dialog_run (GTK_DIALOG (dialog));
    while(result == GTK_RESPONSE_ACCEPT) {
        int code;
        auto user_name = gtk_entry_get_text(GTK_ENTRY(priv.username_entry));
        auto password = gtk_entry_get_text(GTK_ENTRY(priv.password_entry));
        if(strlen(user_name)==0 || strlen(password)==0){
            gtk_label_set_text(GTK_LABEL(warning_lable), "User name or password shouldn't be empty.");
            gtk_entry_set_text(GTK_ENTRY(priv.password_entry), "");
            result = gtk_dialog_run (GTK_DIALOG (dialog));
            continue;
        }
        std::string tkn = getToken(user_name, password, code);
        if(code == 200){
            success = true;
            SaveToken(db, user_name, tkn.c_str());
            strcpy(token, tkn.c_str());
            break;
        } else{
            gtk_label_set_text(GTK_LABEL(warning_lable), "Wrong User name or password.");
            gtk_entry_set_text(GTK_ENTRY(priv.password_entry), "");
            result = gtk_dialog_run (GTK_DIALOG (dialog));
        }
    }
    gtk_widget_destroy (priv.username_entry);
    gtk_widget_destroy (priv.password_entry);
    gtk_widget_destroy (dialog);
    return success;
}
#endif //INNOMETRICS_AUTHORIZATION_H
