//
// Created by alon on 16.03.17.
//

#ifndef INNOMETRICS_SETTINGDIALOG_H
#define INNOMETRICS_SETTINGDIALOG_H
#include <gtk/gtk.h>
#include "Helpers/ConfigParser.h"

struct SettingDialogPrivate
{
    GtkWidget *updateUrl;
    GtkWidget *serverUrl;
    GtkWidget *manual;
    GtkWidget *afterHours;
    Config    *config;
};

struct SettingDialogParameters{
    GtkWidget *window;
    Config* config;
};

static void
activate_manual_switch (GObject    *switcher,
             GParamSpec *pspec,
             gpointer    user_data)
{
    struct SettingDialogPrivate *priv = (SettingDialogPrivate *) user_data;

    if (gtk_switch_get_active (GTK_SWITCH (switcher))){
        gtk_widget_set_sensitive((*priv).afterHours, FALSE);
        (*priv).config->manual = true;
    }
    else{
        gtk_widget_set_sensitive((*priv).afterHours, TRUE);
        (*priv).config->manual = false;
    }
}



void show_settings(GtkWidget *menuItem, gpointer p){
    GtkWidget *dialog, *content_area, *grid, *label, *separator;
    GtkDialogFlags flags;
    struct SettingDialogPrivate priv;

    SettingDialogParameters *param = (SettingDialogParameters *) p;

    priv.config = (*param).config;

    flags = static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT);

    dialog = gtk_dialog_new_with_buttons ("Settings",
                                          GTK_WINDOW((*param).window),
                                          flags,
                                          ("_Ok"),
                                          GTK_RESPONSE_ACCEPT,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          NULL);
    gtk_widget_set_size_request(dialog, 400, 300);
    gtk_window_set_resizable (GTK_WINDOW(dialog), FALSE);

    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

    int cnt= 0;
    grid = gtk_grid_new ();
    g_object_set (grid, "margin", 6, NULL);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 6);
    gtk_grid_set_row_spacing (GTK_GRID (grid), 4);

//region < Update Url >

    label = gtk_label_new("Update Url");
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    gtk_grid_attach (GTK_GRID (grid), label, 0, cnt++, 1, 1);

    priv.updateUrl = gtk_entry_new ();
    gtk_widget_set_hexpand (priv.updateUrl, TRUE);
    gtk_entry_set_text(GTK_ENTRY(priv.updateUrl), priv.config->update_url.c_str());
    gtk_grid_attach (GTK_GRID (grid), priv.updateUrl, 0, cnt++, 1, 1);

//endregion < Update Url>

    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach (GTK_GRID (grid), separator, 0, cnt++, 1, 1);

//region < Server Url >

    label = gtk_label_new("Server Url");
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    gtk_grid_attach (GTK_GRID (grid), label, 0, cnt++, 1, 1);

    priv.serverUrl = gtk_entry_new ();
    gtk_widget_set_hexpand (priv.serverUrl, TRUE);
    gtk_entry_set_text(GTK_ENTRY(priv.serverUrl), priv.config->server_url.c_str());
    gtk_grid_attach (GTK_GRID (grid), priv.serverUrl, 0, cnt++, 1, 1);

//endregion < Server Url>

//region < Deletion option >


    label = gtk_label_new("Manual");
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    gtk_grid_attach (GTK_GRID (grid), label, 0, cnt++, 1, 1);

    priv.manual = gtk_switch_new ();
    gtk_widget_set_halign (priv.manual, GTK_ALIGN_START);
    gtk_switch_set_active (GTK_SWITCH(priv.manual), priv.config->manual);

    g_signal_connect (GTK_SWITCH (priv.manual), "notify::active", G_CALLBACK (activate_manual_switch), &priv);

    gtk_grid_attach (GTK_GRID (grid), priv.manual, 0, cnt++, 1, 1);


    label = gtk_label_new("Count of days");
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    gtk_grid_attach (GTK_GRID (grid), label, 0, cnt++, 1, 1);


    GtkAdjustment *adjustment = gtk_adjustment_new (priv.config->hours, 0.0, 1000.0, 1.0, 5.0, 0.0);

    priv.afterHours = gtk_spin_button_new (adjustment, 1.0, 0);
    gtk_widget_set_hexpand (priv.afterHours, TRUE);
    if(priv.config->manual)
        gtk_widget_set_sensitive(priv.afterHours, FALSE);
    gtk_grid_attach (GTK_GRID (grid), priv.afterHours, 0, cnt++, 1, 1);


//endregion < Deletion Url>


    gtk_box_pack_start (GTK_BOX (content_area), grid, TRUE, TRUE, 0);
    gtk_widget_show_all (dialog);

    gint result = gtk_dialog_run (GTK_DIALOG (dialog));

    if(result == GTK_RESPONSE_ACCEPT){
        priv.config->server_url = gtk_entry_get_text(GTK_ENTRY(priv.serverUrl));
        priv.config->update_url = gtk_entry_get_text(GTK_ENTRY(priv.updateUrl));
        priv.config->hours = (int) gtk_adjustment_get_value(adjustment);
        saveConfig(*priv.config);
        loadConfig(*priv.config);
    }else{

    }

    gtk_widget_destroy (priv.serverUrl);
    gtk_widget_destroy (priv.updateUrl);
    gtk_widget_destroy (priv.manual);
    gtk_widget_destroy (priv.afterHours);

    gtk_widget_destroy (dialog);
}


#endif //INNOMETRICS_SETTINGDIALOG_H
