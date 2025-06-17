#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    GtkEntry    *entry_length;
    GtkEntry    *entry_count;
    GtkTextBuffer *text_buffer;
    GtkLabel    *label_strength;
    GtkToggleButton *chk_lower;
    GtkToggleButton *chk_upper;
    GtkToggleButton *chk_digits;
    GtkToggleButton *chk_symbols;
} AppWidgets;

const char mal[]  = "abcdefghijklmnopqrstuvwxyz";
const char vel[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char cyf[]  = "0123456789";
const char symb[] = "!@#$%^&*";

static char *genp(int length, const char *bazsymb) {
    int n = strlen(bazsymb);
    if (n == 0) return NULL;
    char *p = malloc(length + 1);
    for (int i = 0; i < length; i++)
        p[i] = bazsymb[rand() % n];
    p[length] = '\0';
    return p;
}

static int sula(const char *p) {
    int len = strlen(p), lower = 0, upper = 0, digit = 0, sym = 0;
    for (int i = 0; i < len; i++) {
        if (strchr(mal,  p[i])) lower = 1;
        if (strchr(vel,  p[i])) upper = 1;
        if (strchr(cyf,  p[i])) digit = 1;
        if (strchr(symb, p[i])) sym = 1;
    }
    int variety = lower + upper + digit + sym;
    return len * variety;
}

static const char* sulpar(int score) {
    if (score < 20) return "Слабкий";
    if (score < 40) return "Середній";
    if (score < 60) return "Сильний";
    return "Дуже сильний";
}

static void nav(GtkButton *btn, gpointer data) {
    AppWidgets *w = data;
    int length = atoi(gtk_entry_get_text(w->entry_length));
    if (length <= 0 || length > 99) {
        gtk_label_set_text(w->label_strength, "Довжина: 1–99 символів");
        return;
    }

    int count = atoi(gtk_entry_get_text(w->entry_count));
    if (count <= 0 || count > 10) {
        gtk_label_set_text(w->label_strength, "Кількість: 1–10 паролів");
        return;
    }

    gboolean use_lower  = gtk_toggle_button_get_active(w->chk_lower);
    gboolean use_upper  = gtk_toggle_button_get_active(w->chk_upper);
    gboolean use_digits = gtk_toggle_button_get_active(w->chk_digits);
    gboolean use_symbols= gtk_toggle_button_get_active(w->chk_symbols);
    if (!use_lower && !use_upper && !use_digits && !use_symbols) {
        gtk_label_set_text(w->label_strength, "Оберіть хоча б один тип символів");
        return;
    }

    char bazsymb[256] = {0};
    if (use_lower)   strcat(bazsymb, mal);
    if (use_upper)   strcat(bazsymb, vel);
    if (use_digits)  strcat(bazsymb, cyf);
    if (use_symbols) strcat(bazsymb, symb);

    gtk_text_buffer_set_text(w->text_buffer, "", -1);
    char out[4096] = {0};
    for (int i = 0; i < count; i++) {
        char *pwd = genp(length, bazsymb);
        if (!pwd) continue;
        char line[256];
        snprintf(line, sizeof(line), "%d: %s\n", i + 1, pwd);
        strcat(out, line);
        if (i == count - 1) {
            int score = sula(pwd);
            gtk_label_set_text(w->label_strength, sulpar(score));
        }
        free(pwd);
    }
    gtk_text_buffer_set_text(w->text_buffer, out, -1);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    srand(time(NULL));

    AppWidgets *w = g_slice_new(AppWidgets);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Генератор паролів");
    gtk_window_set_default_size(GTK_WINDOW(win), 700, 400);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        "window { background-color: #2E3440; }"
        "label  { color: #D8DEE9; font-weight: bold; font-size: 16px; }"
        "entry, textview { background-color: #3B4252; color: #D8DEE9; "
                         "font-size: 14px; border-radius: 6px; padding: 6px; }"
        "entry:focus, textview:focus { border: 2px solid #88C0D0; }"
        "button { background-color: #81A1C1; color: #2E3440; "
                 "font-weight: bold; font-size: 16px; border-radius: 8px; padding: 10px; }"
        "button:hover   { background-color: #5E81AC; }"
        "button:pressed { background-color: #4C6781; }"
        "checkbutton    { color: #D8DEE9; font-size: 14px; }"
        , -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(win), grid);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);

    w->entry_length = GTK_ENTRY(gtk_entry_new());
    gtk_widget_set_hexpand(GTK_WIDGET(w->entry_length), TRUE);

    w->entry_count = GTK_ENTRY(gtk_entry_new());
    gtk_widget_set_hexpand(GTK_WIDGET(w->entry_count), TRUE);

    w->chk_lower = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Малі літери"));
    w->chk_upper = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Великі літери"));
    w->chk_digits = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Цифри"));
    w->chk_symbols = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Спецсимволи"));
    gtk_toggle_button_set_active(w->chk_lower, TRUE);
    gtk_toggle_button_set_active(w->chk_upper, TRUE);
    gtk_toggle_button_set_active(w->chk_digits, TRUE);
    gtk_toggle_button_set_active(w->chk_symbols, TRUE);

    GtkWidget *btn = gtk_button_new_with_label("Згенерувати");
    w->label_strength = GTK_LABEL(gtk_label_new("Сила пароля: -"));

    GtkWidget *tv = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tv), FALSE);
    gtk_widget_set_vexpand(tv, TRUE);
    gtk_widget_set_hexpand(tv, TRUE);
    w->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Довжина пароля:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->entry_length), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Кількість паролів:"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->entry_count), 3, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->chk_lower), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->chk_upper), 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->chk_digits), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->chk_symbols), 3, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), btn, 0, 2, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), tv, 0, 3, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(w->label_strength), 0, 4, 4, 1);

    g_signal_connect(btn, "clicked", G_CALLBACK(nav), w);

    gtk_widget_show_all(win);
    gtk_main();

    g_slice_free(AppWidgets, w);
    g_object_unref(css);
    return 0;
}
