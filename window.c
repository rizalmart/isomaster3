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

#include "isomaster.h"

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
    GtkWidget* icon;
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
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
    //~ icon = gtk_image_new_from_stock(GTK_STOCK_NEW, GTK_ICON_SIZE_NEW);
    //~ gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), icon);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(newIsoCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/File/New");
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(openIsoCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/File/Open");
    
/* OPEN recent submenu */
    rootSubmenu = gtk_image_menu_item_new_with_label(_("Open Recent"));
    icon = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(rootSubmenu), icon);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), rootSubmenu);
    gtk_widget_show(rootSubmenu);
    
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
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(saveOverwriteIsoCbk), NULL);
#endif
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(saveIsoCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/File/Save");
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_PROPERTIES, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(buildImagePropertiesWindow), NULL);
    
    separator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
    gtk_widget_show(separator);
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(closeMainWindowCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/File/Quit");
    /* END FILE menu */
    
    /* VIEW menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_View"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_REFRESH, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(refreshBothViewsCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/View/Refresh");
    
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
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_PROPERTIES, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(showBootInfoCbk), NULL);
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(extractBootRecordCbk), NULL);
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_DELETE, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(deleteBootRecordCbk), NULL);
    
    rootSubmenu2 = gtk_image_menu_item_new_from_stock(GTK_STOCK_ADD, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), rootSubmenu2);
    gtk_widget_show(rootSubmenu2);
    
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
    
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);
    menuItem = gtk_menu_item_new_with_mnemonic(_("_Options"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(showPreferencesWindowCbk), (gpointer)BOOT_MEDIA_1_44_FLOPPY);
    /* END TOOLS menu */
    
    /* HELP menu */
    rootMenu = gtk_menu_item_new_with_mnemonic(_("_Help"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), rootMenu);
    gtk_widget_show(rootMenu);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu), menu);
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    icon = gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_MENU);
    menuItem = gtk_image_menu_item_new_with_mnemonic(_("_Overview"));
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), icon);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(showHelpOverviewCbk), NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/Help/Overview");
    
#if GTK_MINOR_VERSION >= 6
    menuItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show(menuItem);
    g_signal_connect(G_OBJECT(menuItem), "activate",
                     G_CALLBACK(showAboutWindowCbk), NULL);
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

void loadAppIcon(GdkPixbuf** appIcon)
{
    /* the path ICONPATH is defined in the makefile
    * if this fails i get NULL which is ok */
    *appIcon = gdk_pixbuf_new_from_file(ICONPATH"/isomaster.png", NULL);
}

void loadIcon(GtkWidget** destIcon, const char* srcFile, int size)
{
    GdkPixbuf* pixbuf;
    
    pixbuf = gdk_pixbuf_new_from_file(srcFile, NULL);
    if(pixbuf == NULL)
    /* could not load icon but need one so replace it with 'unknown' from stock  */
    {
        *destIcon = gtk_image_new_from_stock(GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_LARGE_TOOLBAR);
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
    
    loadIcon(&GBLgoBackIcon, ICONPATH"/go-back-kearone.png", size);
    loadIcon(&GBLgoBackIcon2, ICONPATH"/go-back-kearone.png", size);
    loadIcon(&GBLnewDirIcon, ICONPATH"/folder-new-kearone.png", size);
    loadIcon(&GBLnewDirIcon2, ICONPATH"/folder-new-kearone.png", size);
    loadIcon(&GBLaddIcon, ICONPATH"/add2-kearone.png", size);
    loadIcon(&GBLextractIcon, ICONPATH"/extract2-kearone.png", size);
    loadIcon(&GBLdeleteIcon2, ICONPATH"/delete-kearone.png", size);
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
