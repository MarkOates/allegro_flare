#ifndef __AF_DRAWING_INTERFACE_HTML_CANVAS_HEADER
#define __AF_DRAWING_INTERFACE_HTML_CANVAS_HEADER




#include <sstream>
#include <allegro_flare/drawing_interface.h>




namespace allegro_flare
{
   class DrawingInterfaceHTMLCanvas : public DrawingInterface
   {
   private:
      std::stringstream current_document_output;
      static int next_html5_canvas_id;
      int current_canvas_id;
      std::string get_canvas_id();

   public:
      DrawingInterfaceHTMLCanvas();
      ~DrawingInterfaceHTMLCanvas();

      // preparation and content
      virtual void prepare_surface(int surface_width, int surface_height) override;
      virtual void finish_surface() override;
      
      // files and output
      virtual std::string get_output() override;
      virtual bool save_file(std::string file_basename) override;

      // drawing primitives
      virtual void draw_line(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color, float thickness) override;
      virtual void draw_rectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR outline_color, float thickness) override;
      virtual void draw_text(std::string text, float x, float y, float align_x, float align_y, ALLEGRO_COLOR color, std::string font_family, float font_size, ALLEGRO_FONT *font=NULL) override;
      virtual void draw_ustr_chr(int32_t ustr_char, float x, float y, float align_x, float align_y, ALLEGRO_COLOR color, std::string font_family, float font_size, ALLEGRO_FONT *font=NULL) override;
   };
}



#endif
