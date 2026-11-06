/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

/******************************************************************************
* Author:
* Andrew Smith, http://littlesvr.ca/misc/contactandrew.php
*
* Contributors:
* 
******************************************************************************/

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>
#include <unistd.h>
#include <limits.h>

#include "isomaster.h"

/* GTK3: build a menu item that always shows its icon. The old GtkImageMenuItem
   and the gtk-menu-images setting are deprecated/unreliable, so we pack the
   icon and the mnemonic label into a small box ourselves. */
static GtkWidget* newMenuItem(const char* iconName, const char* label,
                              GCallback cb, const char* accelPath, gpointer userData)
{
    GtkWidget* item = gtk_menu_item_new();
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget* lbl = gtk_label_new_with_mnemonic(label);

    gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), item);
    gtk_box_pack_start(GTK_BOX(box), lbl, FALSE, FALSE, 0);
    /* only add an icon if the theme actually provides it, otherwise GTK
       would display the 'missing image' placeholder */
    if(gtk_icon_theme_has_icon(gtk_icon_theme_get_default(), iconName))
    {
        GtkWidget* img = gtk_image_new_from_icon_name(iconName, GTK_ICON_SIZE_MENU);
        gtk_box_pack_start(GTK_BOX(box), img, FALSE, FALSE, 0);
        gtk_box_reorder_child(GTK_BOX(box), lbl, -1); /* keep label after icon */
    }
    gtk_container_add(GTK_CONTAINER(item), box);
    gtk_widget_show_all(item);

    if(cb != NULL)
        g_signal_connect(G_OBJECT(item), "activate", cb, userData);
    if(accelPath != NULL)
        gtk_menu_item_set_accel_path(GTK_MENU_ITEM(item), accelPath);

    return item;
}

/* the label that holds the value of the iso size */
GtkWidget* GBLisoSizeLbl;
/* icon for 'go back' for fs browser */
GtkWidget* GBLgoBackIcon;
/* icon for 'go back' for iso browser */
GtkWidget* GBLgoBackIcon2;
/* icon for 'new directory' for fs browser */
GtkWidget* GBLnewDirIcon;
/* icon for 'new directory' for iso browser */
GtkWidget* GBLnewDirIcon2;
/* 'add to iso' icon */
GtkWidget* GBLaddIcon;
/* 'extract from iso' icon */
GtkWidget* GBLextractIcon;
/* icon for 'delete' for iso browser */
GtkWidget* GBLdeleteIcon2;
/* list of all recently open ISOs (each can be NULL) */
GtkWidget* GBLrecentlyOpenWidgets[5];

extern GtkWidget* GBLmainWindow;
extern AppSettings GBLappSettings;
extern bool GBLisoPaneActive;
extern bool GBLisoChangesProbable;

void buildMenu(GtkWidget* boxToPackInto)
{
    GtkWidget* menuBar;
    GtkWidget* menu;
    GtkWidget* menuItem;
    GtkWidget* separator;
    GtkWidget* checkbox;
    GtkWidget* rootMenu;
    GtkAccelGroup* accelGroup;
    guint accelKey;
    GdkModifierType accelModifier;
    GClosure *closure = NULL;
    GtkWidget* submenu;
    GtkWidget* rootSubmenu;
    GtkWidget* submenu2;
    GtkWidget* rootSubmenu2;
    
    /* KEYBOARD accelerators */
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(GBLmainWindow), accelGroup);
    
    gtk_accelerator_parse("<Control>N", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(newIsoCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/File/New", accelKey, accelModifier);
    
    gtk_accelerator_parse("<Control>O", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(openIsoCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/File/Open", accelKey, accelModifier);
    
    gtk_accelerator_parse("<Control>S", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(saveIsoCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/File/Save", accelKey, accelModifier);
    
    gtk_accelerator_parse("<Control>W", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(closeMainWindowCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    
    gtk_accelerator_parse("<Control>Q", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(closeMainWindowCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/File/Quit", accelKey, accelModifier);
    
    gtk_accelerator_parse("F1", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(showHelpOverviewCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/Help/Overview", accelKey, accelModifier);
    
    gtk_accelerator_parse("F2", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(renameSelectedBtnCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/Contextmenu/Rename", accelKey, accelModifier);
    
    gtk_accelerator_parse("F3", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(viewSelectedBtnCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/Contextmenu/View", accelKey, accelModifier);
    
    gtk_accelerator_parse("F4", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(editSelectedBtnCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/Contextmenu/Edit", accelKey, accelModifier);
    
    gtk_accelerator_parse("F5", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(refreshBothViewsCbk), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    gtk_accel_map_add_entry("<ISOMaster>/View/Refresh", accelKey, accelModifier);
    /* END KEYBOARD accelerators */
    
    menuBar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(boxToPackInto), menuBar, FALSE, FALSE, 0);
    gtk_widget_show(menuBar);
    
    /* FILE menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_File"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    menuItem = newMenuItem("document-new", _("_New"),
                           G_CALLBACK(newIsoCbk), "<ISOMaster>/File/New", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
    menuItem = newMenuItem("document-open", _("_Open"),
                           G_CALLBACK(openIsoCbk), "<ISOMaster>/File/Open", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
/* OPEN recent submenu */
    rootSubmenu = newMenuItem("document-open", _("Open _Recent"), NULL, NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), rootSubmenu);
    
    submenu = gtk_menu_new();
    
    for(int i = 0; i < 5; i++)
    {
        GBLrecentlyOpenWidgets[i] = gtk_menu_item_new_with_label("");
        gtk_menu_shell_append(GTK_MENU_SHELL(submenu), GBLrecentlyOpenWidgets[i]);
        g_signal_connect(G_OBJECT(GBLrecentlyOpenWidgets[i]), "activate",
                         G_CALLBACK(openRecentIso), NULL);
                         
        if(GBLappSettings.recentlyOpen[i] != NULL)
        {
            gtk_label_set_text(GTK_LABEL(
                    gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[i]))),
                    GBLappSettings.recentlyOpen[i]);
            gtk_widget_show(GBLrecentlyOpenWidgets[i]);
        }

    }
    
     gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootSubmenu), submenu);
    
    /* END OPEN recent submenu */
    
#ifdef ENABLE_SAVE_OVERWRITE
    menuItem = newMenuItem("document-save", _("_Save"),
                           G_CALLBACK(saveOverwriteIsoCbk), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#endif
    
    menuItem = newMenuItem("document-save-as", _("Save _As"),
                           G_CALLBACK(saveIsoCbk), "<ISOMaster>/File/Save", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
    menuItem = newMenuItem("document-properties", _("P_roperties"),
                           G_CALLBACK(buildImagePropertiesWindow), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
    separator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
    gtk_widget_show(separator);
    
    menuItem = newMenuItem("application-exit", _("_Quit"),
                           G_CALLBACK(closeMainWindowCbk), "<ISOMaster>/File/Quit", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    /* END FILE menu */
    
    /* VIEW menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_View"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    menuItem = newMenuItem("view-refresh", _("_Refresh"),
                           G_CALLBACK(refreshBothViewsCbk), "<ISOMaster>/View/Refresh", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
    checkbox = gtk_check_menu_item_new_with_mnemonic(_("Show _hidden files"));
    if(GBLappSettings.showHiddenFilesFs)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), TRUE);
    else
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), FALSE);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), checkbox);
    gtk_widget_show(checkbox);
    g_signal_connect(G_OBJECT(checkbox), "activate",
                     G_CALLBACK(showHiddenCbk), NULL);
    
    checkbox = gtk_check_menu_item_new_with_mnemonic(_("_Sort directories first"));
    if(GBLappSettings.sortDirectoriesFirst)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), TRUE);
    else
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), FALSE);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), checkbox);
    gtk_widget_show(checkbox);
    g_signal_connect(G_OBJECT(checkbox), "activate",
                     G_CALLBACK(sortDirsFirstCbk), NULL);
    
    checkbox = gtk_check_menu_item_new_with_mnemonic(_("Sort is _case sensitive"));
    if(GBLappSettings.caseSensitiveSort)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), TRUE);
    else
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkbox), FALSE);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), checkbox);
    gtk_widget_show(checkbox);
    g_signal_connect(G_OBJECT(checkbox), "activate",
                     G_CALLBACK(caseSensitiveSortCbk), NULL);
    /* END VIEW menu */
    
    /* TOOLS menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_Tools"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    
    /* BOOT submenu */
    rootSubmenu = gtk_menu_item_new_with_mnemonic(_("_Boot Record"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), rootSubmenu);
    gtk_widget_show(rootSubmenu);
    
    submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootSubmenu), submenu);
    
    menuItem = newMenuItem("document-properties", _("P_roperties"),
                           G_CALLBACK(showBootInfoCbk), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    
    menuItem = newMenuItem("document-save-as", _("Save _As"),
                           G_CALLBACK(extractBootRecordCbk), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    
    menuItem = newMenuItem("edit-delete", _("_Delete"),
                           G_CALLBACK(deleteBootRecordCbk), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    
    rootSubmenu2 = newMenuItem("list-add", _("Add"), NULL, NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), rootSubmenu2);
    
    submenu2 = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootSubmenu2), submenu2);
    
    menuItem = gtk_menu_item_new_with_label(_("Use selected file on image (no emulation)"));
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu2), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(setFileAsBootRecordCbk), NULL);
    
    menuItem = gtk_menu_item_new_with_label(_("From file: no emulation"));
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu2), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(addBootRecordFromFileCbk), (gpointer)BOOT_MEDIA_NO_EMULATION);
    
    menuItem = gtk_menu_item_new_with_label(_("From file: 1200KiB floppy"));
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu2), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(addBootRecordFromFileCbk), (gpointer)BOOT_MEDIA_1_2_FLOPPY);
    
    menuItem = gtk_menu_item_new_with_label(_("From file: 1440KiB floppy"));
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu2), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(addBootRecordFromFileCbk), (gpointer)BOOT_MEDIA_1_44_FLOPPY);
    
    menuItem = gtk_menu_item_new_with_label(_("From file: 2880KiB floppy"));
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu2), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(addBootRecordFromFileCbk), (gpointer)BOOT_MEDIA_2_88_FLOPPY);
    /* END BOOT submenu */
    
    menuItem = newMenuItem("preferences-system", _("_Options"),
                           G_CALLBACK(showPreferencesWindowCbk), NULL,
                           (gpointer)BOOT_MEDIA_1_44_FLOPPY);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    /* END TOOLS menu */
    
    /* HELP menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_Help"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    menuItem = newMenuItem("help-contents", _("_Overview"),
                           G_CALLBACK(showHelpOverviewCbk), "<ISOMaster>/Help/Overview", NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    
#if GTK_MINOR_VERSION >= 6
    menuItem = newMenuItem("help-about", _("_About"),
                           G_CALLBACK(showAboutWindowCbk), NULL, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#endif
    /* END HELP menu */
}


void buildMainToolbar(GtkWidget* boxToPackInto)
{
    GtkToolbar* toolbar;
    
    toolbar = gtk_toolbar_new();
        
	GtkWidget *toolbutton_back, *toolbutton_mkdir;
	
    toolbutton_back = gtk_tool_button_new(NULL, _("Go back"));

    gtk_widget_set_tooltip_text(toolbutton_back, _("Go back up one directory on the filesystem"));
	gtk_tool_button_set_icon_widget(toolbutton_back, GBLgoBackIcon);
    g_signal_connect(toolbutton_back, "clicked", G_CALLBACK(fsGoUpDirTreeCbk), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbutton_back, -1);	
	
    toolbutton_mkdir = gtk_tool_button_new(NULL, _("New Directory"));

    gtk_widget_set_tooltip_text(toolbutton_mkdir, _("Create new directory on the filesystem"));
	gtk_tool_button_set_icon_widget(toolbutton_mkdir, GBLnewDirIcon);
    g_signal_connect(toolbutton_mkdir, "clicked", G_CALLBACK(createDirCbk), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbutton_mkdir, -1);	
	
	gtk_box_pack_start(GTK_BOX(boxToPackInto), toolbar, FALSE, FALSE, 0);
    gtk_widget_show(toolbar);
	
}


void buildMiddleToolbar(GtkWidget* boxToPackInto)
{
    GtkToolbar* toolbar;
    GtkWidget* sizeTitleLabel;
    
    toolbar = gtk_toolbar_new();
                            
    GtkWidget *tool_button_back = gtk_tool_button_new(NULL, _("Go back"));
    gtk_widget_set_tooltip_text(tool_button_back, _("Go back up one directory on the ISO"));
    gtk_tool_button_set_icon_widget(tool_button_back, GBLgoBackIcon2);
    g_signal_connect(tool_button_back, "clicked", G_CALLBACK(isoGoUpDirTreeCbk), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_button_back, -1);                                              
                            
    GtkWidget *tool_button_mkdir = gtk_tool_button_new(NULL, _("New Directory"));
    gtk_widget_set_tooltip_text(tool_button_mkdir, _("Create new directory on the ISO"));
    gtk_tool_button_set_icon_widget(tool_button_mkdir, GBLnewDirIcon2);    
    g_signal_connect(tool_button_mkdir, "clicked", G_CALLBACK(createDirCbk), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_button_mkdir, -1);  


    GtkWidget *tool_button_add = gtk_tool_button_new(NULL, _("Add"));
    gtk_widget_set_tooltip_text(tool_button_add, _("Add to the ISO"));
	gtk_tool_button_set_icon_widget(tool_button_add, GBLaddIcon);        
    g_signal_connect(tool_button_add, "clicked", G_CALLBACK(addToIsoCbk), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_button_add, -1); 

                            
    GtkWidget *tool_button_extract = gtk_tool_button_new(NULL, _("Extract"));
    gtk_widget_set_tooltip_text(tool_button_extract, _("Extract from the ISO"));
    gtk_tool_button_set_icon_widget(tool_button_extract, GBLextractIcon); 
    g_signal_connect(tool_button_extract, "clicked", G_CALLBACK(extractFromIsoCbk), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_button_extract, -1); 
                        
    GtkWidget *tool_button_delete = gtk_tool_button_new(NULL, _("Remove"));
    gtk_widget_set_tooltip_text(tool_button_delete, _("Delete from the ISO"));
    gtk_tool_button_set_icon_widget(tool_button_delete, GBLdeleteIcon2);     
    g_signal_connect(tool_button_delete, "clicked", G_CALLBACK(deleteFromIsoCbk), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_button_delete, -1); 


	GtkWidget *label_item1 = gtk_tool_item_new();
    gtk_container_add(GTK_CONTAINER(toolbar), label_item1);
    
    sizeTitleLabel = gtk_label_new(_("      Estimated ISO Size: "));
    gtk_container_add(GTK_CONTAINER(label_item1), sizeTitleLabel);
    gtk_widget_show(sizeTitleLabel);
    

	GtkWidget *label_item2 = gtk_tool_item_new();
    gtk_container_add(GTK_CONTAINER(toolbar), label_item2);
    
    GBLisoSizeLbl = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(label_item2), GBLisoSizeLbl);
    gtk_widget_show(GBLisoSizeLbl);   
    
    gtk_box_pack_start(GTK_BOX(boxToPackInto), toolbar, FALSE, TRUE, 0);
    gtk_widget_show(toolbar);
    
}



void caseSensitiveSortCbk(GtkButton *button, gpointer data)
{
    GBLappSettings.caseSensitiveSort = !GBLappSettings.caseSensitiveSort;
    
    refreshFsView();
    if(GBLisoPaneActive)
        refreshIsoView();
}

gboolean closeMainWindowCbk(GtkWidget *widget, GdkEvent *event)
{
    if(GBLisoChangesProbable && !confirmCloseIso())
        return TRUE;
    
    writeSettings();
    
    deleteTempFiles();
    
    printf("Quitting\n");
    gtk_main_quit();
    
    /* the accelerator callback must return true */
    return TRUE;
}

/* Icons are installed into ICONPATH by 'make install'. When running from the
 * build tree that path doesn't exist, so also look in the icons/ directory
 * next to the executable. The returned string is only valid until the next
 * call. */
static const char* findIconFile(const char* fileName)
{
    static char path[PATH_MAX];
    char exePath[PATH_MAX];
    ssize_t len;
    char* lastSlash;

    snprintf(path, sizeof(path), ICONPATH"/%s", fileName);
    if(access(path, R_OK) == 0)
        return path;

    len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if(len > 0)
    {
        exePath[len] = '\0';
        lastSlash = strrchr(exePath, '/');
        if(lastSlash != NULL)
        {
            snprintf(path, sizeof(path), "%.*s/icons/%s",
                     (int)(lastSlash - exePath), exePath, fileName);
            if(access(path, R_OK) == 0)
                return path;
        }
    }

    return path;
}

void loadAppIcon(GdkPixbuf** appIcon)
{
    /* the path ICONPATH is defined in the makefile
    * if this fails i get NULL which is ok */
    *appIcon = gdk_pixbuf_new_from_file(findIconFile("isomaster.png"), NULL);
}

void loadIcon(GtkWidget** destIcon, const char* srcFile, int size)
{
    GdkPixbuf* pixbuf;
    
    pixbuf = gdk_pixbuf_new_from_file(srcFile, NULL);
    if(pixbuf == NULL)
    /* could not load icon but need one so replace it with 'unknown' from stock  */
    {
        *destIcon = gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_LARGE_TOOLBAR);
    }
    else
    /* resize the icon loaded */
    {
        pixbuf = gdk_pixbuf_scale_simple(pixbuf, size, size, GDK_INTERP_HYPER);
        *destIcon = gtk_image_new_from_pixbuf(pixbuf);
    }
}

void loadIcons(void)
{
    int size;
    
    gtk_icon_size_lookup(GTK_ICON_SIZE_LARGE_TOOLBAR, &size, &size);
    
    loadIcon(&GBLgoBackIcon, findIconFile("go-back-kearone.png"), size);
    loadIcon(&GBLgoBackIcon2, findIconFile("go-back-kearone.png"), size);
    loadIcon(&GBLnewDirIcon, findIconFile("folder-new-kearone.png"), size);
    loadIcon(&GBLnewDirIcon2, findIconFile("folder-new-kearone.png"), size);
    loadIcon(&GBLaddIcon, findIconFile("add2-kearone.png"), size);
    loadIcon(&GBLextractIcon, findIconFile("extract2-kearone.png"), size);
    loadIcon(&GBLdeleteIcon2, findIconFile("delete-kearone.png"), size);
}

void rejectDialogCbk(GtkWidget *widget, GdkEvent *event)
{
    gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_REJECT);
}

void sortDirsFirstCbk(GtkButton *button, gpointer data)
{
    GBLappSettings.sortDirectoriesFirst = !GBLappSettings.sortDirectoriesFirst;
    
    refreshFsView();
    if(GBLisoPaneActive)
        refreshIsoView();
}
