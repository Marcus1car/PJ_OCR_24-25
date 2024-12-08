#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include "../preprocessing/preprocess.h"
#include "../preprocessing/man_rota.c"
#include "../neural_network/core/lib/ocr.h"
#include "../neural_network/core/lib/core_network.h"
#include "../solver/solver.h"


typedef struct UserInterface 
{
    GtkWindow* first_window;
    GtkWindow* main_window;
    GtkWindow* rotate_window;
    GtkWidget* file_chooser;
    GtkWidget* file_chooser_nn;
    GtkButton* file_select_button;
    GtkButton* solve_button;
    GtkButton* open_rotate_window_button;
    GtkButton* preprocess_button;
    GtkButton* grid_detector_button;
    GtkButton* remove_button;
    GtkButton* rotate_button;
    GtkButton* select_file_select_button;
    GtkButton* select_file_cancel_button;
    GtkButton* select_file_cancel_button_nn;
    GtkButton* select_file_select_button_nn;
    GtkEntry* degree_entry;
    GtkImage* displayed_image;
    const char* filename;
    const char* degree;
    const char* filename_nn;
    Network* neural_network;


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

int is_all_digits(const char *str) 
{
    if (str == NULL || *str == '\0')
    {
        return 0; 
    }

    while (*str) 
    { 
        if (!isdigit((unsigned char)*str)) 
        {
            return 0; 
        }
        str++; 
    }
    return 1; 
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
    
    

}

void open_rotate_window(GtkButton *button, gpointer user_data) 
{
    UserInterface* UI = user_data;
    gtk_window_present(GTK_WINDOW(UI->rotate_window));
    gtk_widget_show_all(GTK_WIDGET(UI->rotate_window));

}

void file_select(GtkButton *button, gpointer user_data)
{

    UserInterface* UI = user_data;

    gtk_widget_show(GTK_WIDGET(UI->file_chooser));

    
}

void file_select_nn(GtkButton *button, gpointer user_data)
{

    UserInterface* UI = user_data;

    gtk_widget_show(GTK_WIDGET(UI->file_chooser_nn));

    
}



void select_file_select_nn(GtkButton *button, gpointer user_data)
{

    UserInterface* UI = user_data;

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(UI->file_chooser_nn);
    UI->filename_nn = gtk_file_chooser_get_filename(chooser);
    UI->neural_network = load_nn_data(UI->filename_nn);
    gtk_widget_set_visible(GTK_WIDGET(UI->solve_button), TRUE);
    gtk_widget_hide(GTK_WIDGET(UI->file_chooser_nn));
        

}
void select_file_select(GtkButton *button, gpointer user_data)
{

    UserInterface* UI = user_data;

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(UI->file_chooser);
        UI->filename = gtk_file_chooser_get_filename(chooser);
        
        if (g_file_test(UI->filename, G_FILE_TEST_EXISTS)) 
        {
            gtk_image_set_from_file(UI->displayed_image, UI->filename);
            gtk_window_present(GTK_WINDOW(UI->main_window));
            gtk_widget_show_all(GTK_WIDGET(UI->main_window));
            gtk_widget_hide(GTK_WIDGET(UI->first_window));
            gtk_widget_hide(GTK_WIDGET(UI->solve_button));
            gtk_widget_hide(GTK_WIDGET(UI->grid_detector_button));

        }
        else 
        {
            g_print("File do not exist : %s\n", UI->filename);
        }
    gtk_widget_hide(GTK_WIDGET(UI->file_chooser));
}

void remove_image(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;

    gtk_image_clear(UI->displayed_image);
    gtk_window_present(GTK_WINDOW(UI->first_window));
    gtk_widget_show_all(GTK_WIDGET(UI->first_window));
    gtk_widget_hide(GTK_WIDGET(UI->main_window));
}


void rotate_button_f(GtkButton *button, gpointer user_data)
{
    UserInterface* UI = user_data;
    UI->degree = gtk_entry_get_text(UI->degree_entry);
    if (is_all_digits(UI->degree))
    {
        int k = atoi(UI->degree);
        double j = (double)k;
        SDL_Surface* m = gtk_image_to_sdl_surface(UI->displayed_image);
        m = manualrota(m,j);
        update_displayed_image_with_sdl(UI,m);



    }

}

void select_file_cancel(GtkButton *button, gpointer user_data) 
{
    UserInterface* UI = user_data;
    gtk_widget_hide(UI->file_chooser); 
}

void select_file_cancel_nn(GtkButton *button, gpointer user_data) 
{
    UserInterface* UI = user_data;
    gtk_widget_hide(UI->file_chooser_nn); 
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
    GtkWindow* first_window = GTK_WINDOW(gtk_builder_get_object(builder, "first_window"));
    GtkWindow* main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    GtkWindow* rotate_window = GTK_WINDOW(gtk_builder_get_object(builder, "rotate_window"));
    GtkWidget* file_chooser = GTK_WIDGET(gtk_builder_get_object(builder, "file_chooser"));
    GtkWidget* file_chooser_nn = GTK_WIDGET(gtk_builder_get_object(builder, "file_chooser_nn"));

    GtkButton* file_select_button = GTK_BUTTON(gtk_builder_get_object(builder, "file_select_button"));
    GtkButton* file_select_button_nn = GTK_BUTTON(gtk_builder_get_object(builder, "file_select_button_nn"));
    GtkButton* solve_button = GTK_BUTTON(gtk_builder_get_object(builder, "solve_button"));
    GtkButton* open_rotate_window_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_rotate_window_button"));
    GtkButton* preprocess_button = GTK_BUTTON(gtk_builder_get_object(builder, "preprocess_button"));
    GtkButton* grid_detector_button = GTK_BUTTON(gtk_builder_get_object(builder, "grid_detector_button"));
    GtkButton* remove_button = GTK_BUTTON(gtk_builder_get_object(builder, "remove_button"));
    GtkButton* rotate_button = GTK_BUTTON(gtk_builder_get_object(builder, "rotate_button"));
    GtkButton* select_file_cancel_button = GTK_BUTTON(gtk_builder_get_object(builder, "select_file_cancel_button"));
    GtkButton* select_file_select_button = GTK_BUTTON(gtk_builder_get_object(builder, "select_file_select_button"));
    GtkButton* select_file_cancel_button_nn = GTK_BUTTON(gtk_builder_get_object(builder, "select_file_cancel_button_nn"));
    GtkButton* select_file_select_button_nn = GTK_BUTTON(gtk_builder_get_object(builder, "select_file_select_button_nn"));

    GtkEntry* degree_entry = GTK_ENTRY(gtk_builder_get_object(builder, "degree_entry"));


    GtkImage* displayed_image = GTK_IMAGE(gtk_builder_get_object(builder, "displayed_image"));


    // Structure UserInterface
    UserInterface UI = 
    {
        .first_window = first_window,
        .main_window = main_window,
        .rotate_window = rotate_window,
        .file_chooser = file_chooser,
        .file_chooser_nn = file_chooser_nn,
        .file_select_button = file_select_button,
        .solve_button = solve_button,
        .open_rotate_window_button = open_rotate_window_button,
        .preprocess_button = preprocess_button,
        .grid_detector_button = grid_detector_button,
        .remove_button = remove_button,
        .rotate_button = rotate_button,
        .degree_entry = degree_entry,
        .displayed_image = displayed_image,
        .select_file_select_button = select_file_select_button,
        .select_file_cancel_button = select_file_cancel_button,
        .select_file_cancel_button_nn = select_file_cancel_button_nn,
        .select_file_select_button_nn = select_file_select_button_nn,
        .neural_network = NULL,

    };

    // Connect the signals
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(first_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(solve_button, "clicked", G_CALLBACK(solve_grid), &UI);
    g_signal_connect(grid_detector_button, "clicked", G_CALLBACK(show_grid), &UI);
    g_signal_connect(remove_button, "clicked", G_CALLBACK(remove_image), &UI);
    g_signal_connect(preprocess_button, "clicked", G_CALLBACK(preprocess_grid), &UI);
    g_signal_connect(open_rotate_window_button, "clicked", G_CALLBACK(open_rotate_window), &UI);
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(rotate_button_f), &UI);
    g_signal_connect(file_select_button, "clicked", G_CALLBACK(file_select), &UI);
    g_signal_connect(select_file_select_button, "clicked", G_CALLBACK(select_file_select), &UI);
    g_signal_connect(select_file_cancel_button, "clicked", G_CALLBACK(select_file_cancel), &UI);
    g_signal_connect(file_select_button_nn, "clicked", G_CALLBACK(file_select_nn), &UI);
    g_signal_connect(select_file_select_button_nn, "clicked", G_CALLBACK(select_file_select_nn), &UI);
    g_signal_connect(select_file_cancel_button_nn, "clicked", G_CALLBACK(select_file_cancel_nn), &UI);

    // Loop
    gtk_main();
    return 0;
}
