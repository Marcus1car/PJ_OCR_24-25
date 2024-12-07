#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include "../preprocessing/preprocess.h"

typedef enum State 
{
  START,
  DONE,
} State;


typedef struct UserInterface 
{
    GtkWindow* window;
    GtkImage* displayed_image;
    GtkButton* solve_button;
    GtkButton* preprocess_button;
    GtkButton* grid_detector_button;
    GtkButton* upload_button;
    GtkEntry* text_entry;
    GtkButton* open_rotate_windows_button;
    GtkWindow* rotate_window;
    const gchar *filename;
    State state;
    GtkButton* left_button;
    GtkButton* right_button;
    GtkEntry* text_entry_rotate;
} UserInterface;



SDL_Surface* gtk_image_to_sdl_surface(GtkImage* gtk_image) {
    GdkPixbuf* pixbuf = gtk_image_get_pixbuf(gtk_image);
    if (!pixbuf) {
        g_printerr("Error: GtkImage does not contain a GdkPixbuf\n");
        return NULL;
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int has_alpha = gdk_pixbuf_get_has_alpha(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);


    Uint32 rmask, gmask, bmask, amask;
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = has_alpha ? 0x000000ff : 0;
    } else {
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = has_alpha ? 0xff000000 : 0;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixels,
        width,
        height,
        has_alpha ? 32 : 24,
        rowstride,
        rmask,
        gmask,
        bmask,
        amask
    );

    if (!surface) {
        g_printerr("Error creating SDL_Surface: %s\n", SDL_GetError());
        return NULL;
    }


    SDL_Surface* final_surface = SDL_ConvertSurfaceFormat(surface, has_alpha ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24, 0);
    SDL_FreeSurface(surface); 

    if (!final_surface) {
        g_printerr("Error converting SDL_Surface: %s\n", SDL_GetError());
    }

    return final_surface;
}


GtkImage* sdl_surface_to_gtk_image(SDL_Surface* surface) {
    if (!surface) {
        g_printerr("Error: SDL_Surface is NULL\n");
        return NULL;
    }
    int has_alpha = (surface->format->Amask != 0);
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
        (guchar*)surface->pixels,
        has_alpha ? GDK_COLORSPACE_RGB : GDK_COLORSPACE_RGB,
        has_alpha,
        8,
        surface->w,
        surface->h,
        surface->pitch,
        NULL, 
        NULL  
    );
    if (!pixbuf) {
        g_printerr("Error: Could not create GdkPixbuf from SDL_Surface\n");
        return NULL;
    }
    GtkImage* gtk_image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));
    g_object_unref(pixbuf);

    return gtk_image;
}

void update_displayed_image_with_sdl(UserInterface* UI, SDL_Surface* surface) {
    if (!UI || !surface) {
        g_printerr("Error: UI or SDL_Surface is NULL\n");
        return;
    }

    GtkImage* new_image = sdl_surface_to_gtk_image(surface);
    if (!new_image) {
        g_printerr("Error: Failed to convert SDL_Surface to GtkImage\n");
        return;
    }
    gtk_image_clear(UI->displayed_image); 
    gtk_image_set_from_pixbuf(UI->displayed_image, gtk_image_get_pixbuf(new_image));

    gtk_widget_queue_draw(GTK_WIDGET(UI->displayed_image));

    g_object_unref(new_image);
}




void preprocess_grid(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;
    SDL_Surface* k = gtk_image_to_sdl_surface(UI->displayed_image);
    FinalFunc(k);
    update_displayed_image_with_sdl(UI,k);

    gtk_widget_set_visible(GTK_WIDGET(UI->grid_detector_button), TRUE);

}

void solve_grid(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;
    
    // TO DO



}

void show_grid(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;

    // TO DO
    
    gtk_widget_set_visible(GTK_WIDGET(UI->solve_button), TRUE);

}

void open_rotate_window(GtkButton *button, gpointer user_data) 
{
    UserInterface* UI = user_data;
    gtk_window_present(GTK_WINDOW(UI->rotate_window));
    gtk_widget_show_all(GTK_WIDGET(UI->rotate_window));

}

void upload_image(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;

    if (UI->state == START)
    {
        UI->filename = gtk_entry_get_text(GTK_ENTRY(UI->text_entry));
        if (g_file_test(UI->filename, G_FILE_TEST_EXISTS)) 
        {
            gtk_image_set_from_file(UI->displayed_image, UI->filename);
            gtk_button_set_label(UI->upload_button,"Remove Image");
            UI->state = DONE;
            gtk_widget_set_visible(GTK_WIDGET(UI->preprocess_button), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(UI->open_rotate_windows_button), TRUE);
            g_print("Image successfully loaded from: %s\n", UI->filename);
        }
        else 
        {
            g_print("Error: File does not exist or invalid path.\n");
        }
    }
    else
    {

        gtk_image_clear(UI->displayed_image);
        UI->state = START;
        gtk_window_resize(GTK_WINDOW(UI->window), 800, 600);
        gtk_window_set_position(GTK_WINDOW(UI->window), GTK_WIN_POS_CENTER);
        gtk_button_set_label(UI->upload_button,"Upload Image");
        gtk_widget_set_visible(GTK_WIDGET(UI->preprocess_button), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(UI->solve_button), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(UI->grid_detector_button), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(UI->open_rotate_windows_button), FALSE);

    }
}

void on_left_button_clicked(GtkButton *button, gpointer user_data) 
{
    



}

void on_right_button_clicked(GtkButton *button, gpointer user_data) 
{



    
}


int main(int argc, char* argv[]) 
{
    gtk_init(NULL, NULL);
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "gui.glade", &error) == 0) 
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Widgets
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.gui"));
    GtkButton* solve_button = GTK_BUTTON(gtk_builder_get_object(builder, "solve_button"));
    GtkButton* grid_detector_button = GTK_BUTTON(gtk_builder_get_object(builder, "grid_detector_button"));
    GtkButton* upload_button = GTK_BUTTON(gtk_builder_get_object(builder, "upload_button"));
    GtkButton* preprocess_button = GTK_BUTTON(gtk_builder_get_object(builder, "preprocess_button"));
    GtkEntry* text_entry = GTK_ENTRY(gtk_builder_get_object(builder, "text_entry"));
    GtkImage* displayed_image = GTK_IMAGE(gtk_builder_get_object(builder, "displayed_image"));
    GtkButton* open_rotate_windows_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_rotate_windows_button"));
    GtkWindow* rotate_window = GTK_WINDOW(gtk_builder_get_object(builder, "rotate_window"));
    GtkButton* left_button = GTK_BUTTON(gtk_builder_get_object(builder, "left_button"));
    GtkButton* right_button = GTK_BUTTON(gtk_builder_get_object(builder, "right_button"));
    GtkEntry* text_entry_rotate = GTK_ENTRY(gtk_builder_get_object(builder, "text_entry_rotate"));


    // Structure UserInterface
    UserInterface UI = 
    {
        .window = window,
        .solve_button = solve_button,
        .grid_detector_button = grid_detector_button,
        .displayed_image = displayed_image,
        .text_entry = text_entry,
        .upload_button = upload_button,
        .preprocess_button = preprocess_button,
        .filename = NULL,
        .state = START,
        .open_rotate_windows_button = open_rotate_windows_button,
        .rotate_window = rotate_window,
        .left_button = left_button,
        .right_button = right_button,
        .text_entry_rotate = text_entry_rotate,
    };

    // Connect the signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(solve_button, "clicked", G_CALLBACK(solve_grid), &UI);
    g_signal_connect(grid_detector_button, "clicked", G_CALLBACK(show_grid), &UI);
    g_signal_connect(upload_button, "clicked", G_CALLBACK(upload_image), &UI);
    g_signal_connect(preprocess_button, "clicked", G_CALLBACK(preprocess_grid), &UI);
    g_signal_connect(open_rotate_windows_button, "clicked", G_CALLBACK(open_rotate_window), &UI);
    g_signal_connect(left_button, "clicked", G_CALLBACK(on_left_button_clicked), &UI);
    g_signal_connect(right_button, "clicked", G_CALLBACK(on_right_button_clicked), &UI);

    // Loop
    gtk_main();
    return 0;
}
