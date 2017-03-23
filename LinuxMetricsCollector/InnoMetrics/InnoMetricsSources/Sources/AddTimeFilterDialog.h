//
// Created by alon on 18.03.17.
//

#ifndef INNOMETRICS_ADDTIMEFILTERDIALOG_H
#define INNOMETRICS_ADDTIMEFILTERDIALOG_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <cstring>

gchar*
timeify ( const char* cstr, bool &newChar )
{
    const char* c;
    gchar* ret_val;
    GString* s = g_string_new("");

    g_debug( "string = %s", cstr);

    int index;

    for ( c = cstr, index = 0; *c != '\0'; c = g_utf8_next_char(c), index++) {
        gunichar cp = g_utf8_get_char(c);

        if(index>5)
            break;

        if(g_unichar_isdigit(cp)){
            if(index == 2){
                g_string_append_unichar( s, ':' );
            }
            g_string_append_unichar( s, cp );
        }
        else if(cp == ':' && (index == 2)){
            g_string_append_unichar( s, cp );
        }

    }
    if(index == 2){
        g_string_append_unichar( s, ':');
        newChar=true;
    }

    ret_val = s->str;
    g_string_free(s, FALSE);
    return ret_val;
}

void
time_entry_insert_after_handler(GtkEditable *editable,
                                    gchar *new_text,
                                    gint new_length,
                                    gpointer position,
                                    gpointer data)
{
    /*prevent compiler warnings about unused variables*/
    (void) new_text; (void) new_length; (void) position; (void) data;

    const gchar* content = gtk_entry_get_text( GTK_ENTRY(editable) );
    bool newChar = false;
    gchar* modified =  timeify(content, newChar);

    if(strlen(modified)>4){
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strptime(modified, "%H:%M", timeinfo);

        rawtime = mktime(timeinfo);

        strftime (modified,6,"%H:%M",timeinfo);
    }

    gtk_entry_set_text(GTK_ENTRY(editable), modified);

    if(newChar){
        int *pos = (int*)position;
        *pos = *pos + 1;
    }

    g_free(modified);
}

void
time_entry_delete_after_handler(GtkEditable *editable,
                                    gint start_pos,
                                    gint end_pos,
                                    gpointer data)
{
    /*no op cast to prevent compiler warnings*/
    (void) start_pos; (void) end_pos; (void) data;

    int cursor_pos = gtk_editable_get_position(editable);

    /*get text and modify the entry*/
    const gchar* content = gtk_entry_get_text( GTK_ENTRY(editable) );
    bool newChar = false;
    gchar* modified = timeify( content, newChar);
    gtk_entry_set_text(GTK_ENTRY(editable),modified);

    gtk_editable_set_position(editable, cursor_pos);
    g_free(modified);
}

bool
AddTimeFilterDialog(GtkWindow *parent, char *from, char *to) {
    GtkWidget *dialog, *content_area, *label, *fromEntry, *toEntry;
    GtkDialogFlags flags;

    // Create the widgets
    flags = static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT);
    dialog = gtk_dialog_new_with_buttons("Pick time",
                                         parent,
                                         flags,
                                         ("_OK"),
                                         GTK_RESPONSE_ACCEPT,
                                         ("_Cancel"),
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG (dialog));

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    //Content

    GtkWidget *grid;


    grid = gtk_grid_new();
    g_object_set(grid, "margin", 6, NULL);
    gtk_grid_set_column_spacing(GTK_GRID (grid), 6);
    gtk_grid_set_row_spacing(GTK_GRID (grid), 4);
    gtk_box_pack_start(GTK_BOX (content_area), grid, TRUE, TRUE, 0);


    label = gtk_label_new("From:");
    gtk_widget_set_halign(label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID (grid), label, 0, 1, 1, 1);
    fromEntry = gtk_entry_new();
    g_signal_connect_after( fromEntry, "insert-text", G_CALLBACK(time_entry_insert_after_handler), NULL );
    g_signal_connect_after( fromEntry, "delete-text", G_CALLBACK(time_entry_delete_after_handler), NULL );
    gtk_widget_set_hexpand(fromEntry, TRUE);

    timeinfo->tm_hour -= 1;
    mktime(timeinfo);
    strftime (buffer,80,"%H:%M",timeinfo);

    auto gBuffer = gtk_entry_get_buffer (GTK_ENTRY (fromEntry));
    gtk_entry_buffer_set_text(gBuffer, buffer, strlen(buffer));

    gtk_grid_attach(GTK_GRID (grid), fromEntry,
                    1, 1, 1, 1);


    label = gtk_label_new("To:");
    gtk_widget_set_halign(label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID (grid), label, 0, 2, 1, 1);
    toEntry = gtk_entry_new();
    g_signal_connect_after( toEntry, "insert-text", G_CALLBACK(time_entry_insert_after_handler), NULL );
    g_signal_connect_after( toEntry, "delete-text", G_CALLBACK(time_entry_delete_after_handler), NULL );
    gtk_widget_set_hexpand(toEntry, TRUE);

    timeinfo->tm_hour += 1;
    mktime(timeinfo);
    strftime (buffer,80,"%H:%M",timeinfo);

    gBuffer = gtk_entry_get_buffer (GTK_ENTRY (toEntry));
    gtk_entry_buffer_set_text(gBuffer, buffer, strlen(buffer));

    gtk_grid_attach(GTK_GRID (grid), toEntry,
                    1, 2, 1, 1);


    // Ensure that the dialog box is destroyed when the user responds

//    g_signal_connect_swapped (dialog,
//                              "response",
//                              G_CALLBACK (gtk_widget_destroy),
//                              dialog);


    gtk_widget_show_all(dialog);

    bool success = false;

    gint result = gtk_dialog_run(GTK_DIALOG (dialog));
    if(result == GTK_RESPONSE_ACCEPT){
        strcpy(from, gtk_entry_get_text(GTK_ENTRY(fromEntry)));
        strcpy(to, gtk_entry_get_text(GTK_ENTRY(toEntry)));
        success = true;
    }




    gtk_widget_destroy(fromEntry);
    gtk_widget_destroy(toEntry);
    gtk_widget_destroy(dialog);
    return success;
}
#endif //INNOMETRICS_ADDTIMEFILTERDIALOG_H
