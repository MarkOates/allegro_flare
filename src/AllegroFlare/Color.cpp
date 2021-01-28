



#include <AllegroFlare/Color.hpp>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>




namespace AllegroFlare
{
   inline float Color::clamp_color(float v){
      return (v>1.0f) ? 1.0f : ((v<0.0f) ? 0.0f : v);
   }
   
   Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
   {}
   
   Color::Color(ALLEGRO_COLOR allegro_color)
   {
      r = allegro_color.r;
      g = allegro_color.g;
      b = allegro_color.b;
      a = allegro_color.a;
   }
   
   Color::Color(int hex, float alpha)
   {
      r = (float)(hex & 0xFF0000)/0xFF0000;
      g = (float)(hex & 0x00FF00)/0xFF00;
      b = (float)(hex & 0x0000FF)/0xFF;
   }
   
   Color::Color(float r, float g, float b, float a)
   {
      this->r = r;
      this->g = g;
      this->b = b;
      this->a = a;
   }
   
   Color Color::rgba(int r, int g, int b, float a)
   {
      return Color(
         (float)r/255,
         (float)g/255,
         (float)b/255,
         a);
   }
   
   Color::Color(std::string color_name, float alpha)
   {
      ALLEGRO_COLOR color = al_color_name(color_name.c_str());
      r = color.r;
      g = color.g;
      b = color.b;
      a = alpha;
   }
   
   Color Color::mix(Color &c1, Color &c2, float scale)
   {
      Color col;
      col.r = (c2.r - c1.r) * scale + c1.r;
      col.g = (c2.g - c1.g) * scale + c1.g;
      col.b = (c2.b - c1.b) * scale + c1.b;
      col.a = (c2.a - c1.a) * scale + c1.a;
      return col;
   }
   
   Color::~Color()
   {}
   
   Color Color::operator+(const Color &c2){
      return Color(
         clamp_color(r + c2.r),
         clamp_color(g + c2.g),
         clamp_color(b + c2.b),
         clamp_color(a + c2.a)
      );
   }
   
   Color Color::operator-(const Color &c2){
      return Color(
         clamp_color(r - c2.r),
         clamp_color(g - c2.g),
         clamp_color(b - c2.b),
         clamp_color(a - c2.a)
      );
   }
   
   Color Color::operator*(const Color &c2){
      return Color(
         clamp_color(r * c2.r),
         clamp_color(g * c2.g),
         clamp_color(b * c2.b),
         clamp_color(a * c2.a)
      );
   }
   
   Color operator*(const Color &c, float k){
      return Color(
         Color::clamp_color(c.r * k),
         Color::clamp_color(c.g * k),
         Color::clamp_color(c.b * k),
         c.a // alpha value really shouldn't be modified by a multiplication with a float
      );
   }
   
   Color operator*(float k, const Color &c){
      return Color(
         Color::clamp_color(c.r * k),
         Color::clamp_color(c.g * k),
         Color::clamp_color(c.b * k),
         c.a // alpha value really shouldn't be modified by a multiplication with a float
      );
   }
   
   std::ostream& operator<<(std::ostream& os, const Color &c){
      os << "rgba(" << c.r*255 << "," << c.g*255 << "," << c.b*255 << "," << c.a << ")";
      return os;
   }
   
   ALLEGRO_COLOR operator+(const ALLEGRO_COLOR& lhs, const ALLEGRO_COLOR& rhs)
   {
      ALLEGRO_COLOR result;
      result.a = lhs.a + rhs.a;
      result.r = lhs.r + rhs.r;
      result.g = lhs.g + rhs.g;
      result.b = lhs.b + rhs.b;
      return result;
   }




   ALLEGRO_COLOR operator-(const ALLEGRO_COLOR& lhs, const ALLEGRO_COLOR& rhs)
   {
      ALLEGRO_COLOR result;
      result.a = lhs.a - rhs.a;
      result.r = lhs.r - rhs.r;
      result.g = lhs.g - rhs.g;
      result.b = lhs.b - rhs.b;
      return result;
   }




   ALLEGRO_COLOR operator*(const ALLEGRO_COLOR& lhs, const ALLEGRO_COLOR& rhs)
   {
      ALLEGRO_COLOR result;
      result.a = lhs.a * rhs.a;
      result.r = lhs.r * rhs.r;
      result.g = lhs.g * rhs.g;
      result.b = lhs.b * rhs.b;
      return result;
   }




   ALLEGRO_COLOR operator*(const ALLEGRO_COLOR& lhs, float rhs)
   {
      ALLEGRO_COLOR result;
      result.a = lhs.a * rhs;
      result.r = lhs.r * rhs;
      result.g = lhs.g * rhs;
      result.b = lhs.b * rhs;
      return result;
   }




   ALLEGRO_COLOR operator*(float lhs, const ALLEGRO_COLOR& rhs)
   {
      ALLEGRO_COLOR result;
      result.a = lhs * rhs.a;
      result.r = lhs * rhs.r;
      result.g = lhs * rhs.g;
      result.b = lhs * rhs.b;
      return result;
   }




   namespace color
   {
      float blend_op::add(float val1, float val2) { return val1 + val2; }
      float blend_op::subtract(float val1, float val2) { return val1 - val2; }
      float blend_op::multiply(float val1, float val2) { return val1 * val2; }
      float blend_op::divide(float val1, float val2) { return val1 / val2; }




      ALLEGRO_COLOR color(const char *color_name, float alpha) // maybe remove this function?
      {
         ALLEGRO_COLOR col = al_color_name(color_name);
         col.a = alpha;
         col.r *= alpha;
         col.g *= alpha;
         col.b *= alpha;
         return col;
      }




      ALLEGRO_COLOR color(ALLEGRO_COLOR color, float alpha) // maybe rename to "adjust_alpha"
      {
         color.a *= alpha;
         color.r *= alpha;
         color.g *= alpha;
         color.b *= alpha;
         return color;
      }




      ALLEGRO_COLOR hex(const char *hex, float alpha)
      {
         ALLEGRO_COLOR color = al_color_html(hex);
         color.a = alpha;
         color.r *= alpha;
         color.g *= alpha;
         color.b *= alpha;
         return color;
      }




      ALLEGRO_COLOR name(const char *name, float alpha)
      {
         ALLEGRO_COLOR color = al_color_name(name);
         color.a = alpha;
         color.r *= alpha;
         color.g *= alpha;
         color.b *= alpha;
         return color;
      }




      ALLEGRO_COLOR rgb(float r, float g, float b, float alpha)
      {
         return al_map_rgba_f(r, g, b, alpha);
      }




      ALLEGRO_COLOR hsl(float h, float s, float l, float alpha)
      {
         ALLEGRO_COLOR col = al_color_hsl(h, s, l);
         color::change_alpha(col, alpha);
         return col;
      }




      std::string get_css_str(ALLEGRO_COLOR &col)
      {
         std::stringstream ss;
         ss << "rgba(" << (int)(col.r * 255) << ", " << (int)(col.g * 255) << ", " << (int)(col.b * 255) << ", " << col.a << ")";
         return ss.str();
      }




      ALLEGRO_COLOR mix(const ALLEGRO_COLOR &col1, const ALLEGRO_COLOR &col2, float scale)
      {
         ALLEGRO_COLOR col;
         col.r = (col2.r - col1.r) * scale + col1.r;
         col.g = (col2.g - col1.g) * scale + col1.g;
         col.b = (col2.b - col1.b) * scale + col1.b;
         col.a = (col2.a - col1.a) * scale + col1.a;
         return col;
      }




      void change_alpha(ALLEGRO_COLOR &color, float alpha)
      {
         color.a *= alpha;
         color.r *= color.a;
         color.g *= color.a;
         color.b *= color.a;
      }




      void change_hue(ALLEGRO_COLOR &color, float amount, float (*operation)(float op1, float op2))
      {
         float h, s, l, a=color.a;
         al_color_rgb_to_hsl(color.r, color.g, color.b, &h, &s, &l);
         h /= 360.0;
         h = fmod(operation(h, amount), 1.0f);
         //h = min(max(0, ), 1.0);
         color = al_color_hsl(h*360, s, l);
         color.a = a;
      }




      void change_saturation(ALLEGRO_COLOR &color, float amount, float (*operation)(float op1, float op2))
      {
         float h, s, l, a=color.a;
         al_color_rgb_to_hsl(color.r, color.g, color.b, &h, &s, &l);
         s = std::min(std::max(0.0f, operation(s, amount)), 1.0f);
         color = al_color_hsl(h, s, l);
         color.a = a;
      }




      void change_lightness(ALLEGRO_COLOR &color, float amount, float (*operation)(float op1, float op2))
      {
         float h, s, l, a=color.a;
         al_color_rgb_to_hsl(color.r, color.g, color.b, &h, &s, &l);
         l = std::min(std::max(0.0f, operation(l, amount)), 1.0f);
         color = al_color_hsl(h, s, l);
         color.a = a;
      }




      void change_value(ALLEGRO_COLOR &color, float amount, float (*operation)(float op1, float op2))
      {
         float h, s, v, a=color.a;
         al_color_rgb_to_hsv(color.r, color.g, color.b, &h, &s, &v);
         v = std::min(std::max(0.0f, operation(v, amount)), 1.0f);
         color = al_color_hsl(h, s, v);
         color.a = a;
      }




      float get_lightness(ALLEGRO_COLOR &color)
      {
         float h, s, l;
         al_color_rgb_to_hsl(color.r, color.g, color.b, &h, &s, &l);
         return l;
      }




      void change_hue(ALLEGRO_BITMAP *bitmap, float amount, float (*operation)(float op1, float op2))
      {
         adjust_image(bitmap, amount, color::change_hue, operation);
      }




      void change_saturation(ALLEGRO_BITMAP *bitmap, float amount, float (*operation)(float op1, float op2))
      {
         adjust_image(bitmap, amount, color::change_saturation, operation);
      }




      void change_lightness(ALLEGRO_BITMAP *bitmap, float amount, float (*operation)(float op1, float op2))
      {
         adjust_image(bitmap, amount, color::change_lightness, operation);
      }




      void change_value(ALLEGRO_BITMAP *bitmap, float amount, float (*operation)(float op1, float op2))
      {
         adjust_image(bitmap, amount, color::change_value, operation);
      }




      void change_to_color(ALLEGRO_BITMAP *bitmap, float __amount_discarded, float (*operation)(float op1, float op2))
      {

      }




      void adjust_image(ALLEGRO_BITMAP *bitmap, float amount,
            void (*filter_func)(ALLEGRO_COLOR &, float, float (*)(float, float)),
            float (*filter_func_op)(float op1, float op2))
      {
         ALLEGRO_STATE state;
         al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
         al_set_target_bitmap(bitmap);
         al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_8888, ALLEGRO_LOCK_READWRITE);
         for (int x=0; x<al_get_bitmap_width(bitmap); x++)
            for (int y=0; y<al_get_bitmap_height(bitmap); y++)
            {
               ALLEGRO_COLOR get_color = al_get_pixel(bitmap, x, y);
               filter_func(get_color, amount, filter_func_op);
               al_put_pixel(x, y, get_color);
            }
         al_unlock_bitmap(bitmap);
         al_restore_state(&state);
      }




      /*
         static void adjust_image(ALLEGRO_BITMAP *bitmap, float (*filter_func)(float))
         {
         }
         */




      // TODO: fix for potential floating point errors
      bool operator ==(const ALLEGRO_COLOR &lhs, const ALLEGRO_COLOR &rhs)
      {
         return color::basically_equal(lhs, rhs);
      }




      bool basically_equal(const ALLEGRO_COLOR &col1, const ALLEGRO_COLOR &col2, float threshold)
      {
         if (std::abs(col1.a - col2.a) > threshold) return false;
         if (std::abs(col1.g - col2.g) > threshold) return false;
         if (std::abs(col1.b - col2.b) > threshold) return false;
         if (std::abs(col1.r - col2.r) > threshold) return false;
         return true;
      }




      const ALLEGRO_COLOR null_color = al_map_rgba_f(0, 0, 0, 1.0);
      const ALLEGRO_COLOR magic_pink = al_map_rgba_f(1, 0, 1, 1.0);
      const ALLEGRO_COLOR transparent = al_map_rgba_f(0, 0, 0, 0);
      const ALLEGRO_COLOR aliceblue = al_color_name("aliceblue");
      const ALLEGRO_COLOR antiquewhite = al_color_name("antiquewhite");
      const ALLEGRO_COLOR aqua = al_color_name("aqua");
      const ALLEGRO_COLOR aquamarine = al_color_name("aquamarine");
      const ALLEGRO_COLOR azure = al_color_name("azure");
      const ALLEGRO_COLOR beige = al_color_name("beige");
      const ALLEGRO_COLOR bisque = al_color_name("bisque");
      const ALLEGRO_COLOR black = al_color_name("black");
      const ALLEGRO_COLOR blanchedalmond = al_color_name("blanchedalmond");
      const ALLEGRO_COLOR blue = al_color_name("blue");
      const ALLEGRO_COLOR blueviolet = al_color_name("blueviolet");
      const ALLEGRO_COLOR brown = al_color_name("brown");
      const ALLEGRO_COLOR burlywood = al_color_name("burlywood");
      const ALLEGRO_COLOR cadetblue = al_color_name("cadetblue");
      const ALLEGRO_COLOR chartreuse = al_color_name("chartreuse");
      const ALLEGRO_COLOR chocolate = al_color_name("chocolate");
      const ALLEGRO_COLOR coral = al_color_name("coral");
      const ALLEGRO_COLOR cornflowerblue = al_color_name("cornflowerblue");
      const ALLEGRO_COLOR cornsilk = al_color_name("cornsilk");
      const ALLEGRO_COLOR crimson = al_color_name("crimson");
      const ALLEGRO_COLOR cyan = al_color_name("cyan");
      const ALLEGRO_COLOR darkblue = al_color_name("darkblue");
      const ALLEGRO_COLOR darkcyan = al_color_name("darkcyan");
      const ALLEGRO_COLOR darkgoldenrod = al_color_name("darkgoldenrod");
      const ALLEGRO_COLOR darkgray = al_color_name("darkgray");
      const ALLEGRO_COLOR darkgreen = al_color_name("darkgreen");
      const ALLEGRO_COLOR darkkhaki = al_color_name("darkkhaki");
      const ALLEGRO_COLOR darkmagenta = al_color_name("darkmagenta");
      const ALLEGRO_COLOR darkolivegreen = al_color_name("darkolivegreen");
      const ALLEGRO_COLOR darkorange = al_color_name("darkorange");
      const ALLEGRO_COLOR darkorchid = al_color_name("darkorchid");
      const ALLEGRO_COLOR darkred = al_color_name("darkred");
      const ALLEGRO_COLOR darksalmon = al_color_name("darksalmon");
      const ALLEGRO_COLOR darkseagreen = al_color_name("darkseagreen");
      const ALLEGRO_COLOR darkslateblue = al_color_name("darkslateblue");
      const ALLEGRO_COLOR darkslategray = al_color_name("darkslategray");
      const ALLEGRO_COLOR darkturquoise = al_color_name("darkturquoise");
      const ALLEGRO_COLOR darkviolet = al_color_name("darkviolet");
      const ALLEGRO_COLOR deeppink = al_color_name("deeppink");
      const ALLEGRO_COLOR deepskyblue = al_color_name("deepskyblue");
      const ALLEGRO_COLOR dimgray = al_color_name("dimgray");
      const ALLEGRO_COLOR dodgerblue = al_color_name("dodgerblue");
      const ALLEGRO_COLOR firebrick = al_color_name("firebrick");
      const ALLEGRO_COLOR floralwhite = al_color_name("floralwhite");
      const ALLEGRO_COLOR forestgreen = al_color_name("forestgreen");
      const ALLEGRO_COLOR fuchsia = al_color_name("fuchsia");
      const ALLEGRO_COLOR gainsboro = al_color_name("gainsboro");
      const ALLEGRO_COLOR ghostwhite = al_color_name("ghostwhite");
      const ALLEGRO_COLOR goldenrod = al_color_name("goldenrod");
      const ALLEGRO_COLOR gold = al_color_name("gold");
      const ALLEGRO_COLOR gray = al_color_name("gray");
      const ALLEGRO_COLOR green = al_color_name("green");
      const ALLEGRO_COLOR greenyellow = al_color_name("greenyellow");
      const ALLEGRO_COLOR honeydew = al_color_name("honeydew");
      const ALLEGRO_COLOR hotpink = al_color_name("hotpink");
      const ALLEGRO_COLOR indianred = al_color_name("indianred");
      const ALLEGRO_COLOR indigo = al_color_name("indigo");
      const ALLEGRO_COLOR ivory = al_color_name("ivory");
      const ALLEGRO_COLOR khaki = al_color_name("khaki");
      const ALLEGRO_COLOR lavenderblush = al_color_name("lavenderblush");
      const ALLEGRO_COLOR lavender = al_color_name("lavender");
      const ALLEGRO_COLOR lawngreen = al_color_name("lawngreen");
      const ALLEGRO_COLOR lemonchiffon = al_color_name("lemonchiffon");
      const ALLEGRO_COLOR lightblue = al_color_name("lightblue");
      const ALLEGRO_COLOR lightcoral = al_color_name("lightcoral");
      const ALLEGRO_COLOR lightcyan = al_color_name("lightcyan");
      const ALLEGRO_COLOR lightgoldenrodyellow = al_color_name("lightgoldenrodyellow");
      const ALLEGRO_COLOR lightgreen = al_color_name("lightgreen");
      const ALLEGRO_COLOR lightgrey = al_color_name("lightgrey");
      const ALLEGRO_COLOR lightpink = al_color_name("lightpink");
      const ALLEGRO_COLOR lightsalmon = al_color_name("lightsalmon");
      const ALLEGRO_COLOR lightseagreen = al_color_name("lightseagreen");
      const ALLEGRO_COLOR lightskyblue = al_color_name("lightskyblue");
      const ALLEGRO_COLOR lightslategray = al_color_name("lightslategray");
      const ALLEGRO_COLOR lightsteelblue = al_color_name("lightsteelblue");
      const ALLEGRO_COLOR lightyellow = al_color_name("lightyellow");
      const ALLEGRO_COLOR lime = al_color_name("lime");
      const ALLEGRO_COLOR limegreen = al_color_name("limegreen");
      const ALLEGRO_COLOR linen = al_color_name("linen");
      const ALLEGRO_COLOR magenta = al_color_name("magenta");
      const ALLEGRO_COLOR maroon = al_color_name("maroon");
      const ALLEGRO_COLOR mediumaquamarine = al_color_name("mediumaquamarine");
      const ALLEGRO_COLOR mediumblue = al_color_name("mediumblue");
      const ALLEGRO_COLOR mediumorchid = al_color_name("mediumorchid");
      const ALLEGRO_COLOR mediumpurple = al_color_name("mediumpurple");
      const ALLEGRO_COLOR mediumseagreen = al_color_name("mediumseagreen");
      const ALLEGRO_COLOR mediumslateblue = al_color_name("mediumslateblue");
      const ALLEGRO_COLOR mediumspringgreen = al_color_name("mediumspringgreen");
      const ALLEGRO_COLOR mediumturquoise = al_color_name("mediumturquoise");
      const ALLEGRO_COLOR mediumvioletred = al_color_name("mediumvioletred");
      const ALLEGRO_COLOR midnightblue = al_color_name("midnightblue");
      const ALLEGRO_COLOR mintcream = al_color_name("mintcream");
      const ALLEGRO_COLOR mistyrose = al_color_name("mistyrose");
      const ALLEGRO_COLOR moccasin = al_color_name("moccasin");
      const ALLEGRO_COLOR navajowhite = al_color_name("navajowhite");
      const ALLEGRO_COLOR navy = al_color_name("navy");
      const ALLEGRO_COLOR oldlace = al_color_name("oldlace");
      const ALLEGRO_COLOR olive = al_color_name("olive");
      const ALLEGRO_COLOR olivedrab = al_color_name("olivedrab");
      const ALLEGRO_COLOR orange = al_color_name("orange");
      const ALLEGRO_COLOR orangered = al_color_name("orangered");
      const ALLEGRO_COLOR orchid = al_color_name("orchid");
      const ALLEGRO_COLOR palegoldenrod = al_color_name("palegoldenrod");
      const ALLEGRO_COLOR palegreen = al_color_name("palegreen");
      const ALLEGRO_COLOR paleturquoise = al_color_name("paleturquoise");
      const ALLEGRO_COLOR palevioletred = al_color_name("palevioletred");
      const ALLEGRO_COLOR papayawhip = al_color_name("papayawhip");
      const ALLEGRO_COLOR peachpuff = al_color_name("peachpuff");
      const ALLEGRO_COLOR peru = al_color_name("peru");
      const ALLEGRO_COLOR pink = al_color_name("pink");
      const ALLEGRO_COLOR plum = al_color_name("plum");
      const ALLEGRO_COLOR powderblue = al_color_name("powderblue");
      const ALLEGRO_COLOR purple = al_color_name("purple");
      const ALLEGRO_COLOR purwablue = al_color_name("purwablue");
      const ALLEGRO_COLOR red = al_color_name("red");
      const ALLEGRO_COLOR rosybrown = al_color_name("rosybrown");
      const ALLEGRO_COLOR royalblue = al_color_name("royalblue");
      const ALLEGRO_COLOR saddlebrown = al_color_name("saddlebrown");
      const ALLEGRO_COLOR salmon = al_color_name("salmon");
      const ALLEGRO_COLOR sandybrown = al_color_name("sandybrown");
      const ALLEGRO_COLOR seagreen = al_color_name("seagreen");
      const ALLEGRO_COLOR seashell = al_color_name("seashell");
      const ALLEGRO_COLOR sienna = al_color_name("sienna");
      const ALLEGRO_COLOR silver = al_color_name("silver");
      const ALLEGRO_COLOR skyblue = al_color_name("skyblue");
      const ALLEGRO_COLOR slateblue = al_color_name("slateblue");
      const ALLEGRO_COLOR slategray = al_color_name("slategray");
      const ALLEGRO_COLOR snow = al_color_name("snow");
      const ALLEGRO_COLOR springgreen = al_color_name("springgreen");
      const ALLEGRO_COLOR steelblue = al_color_name("steelblue");
      const ALLEGRO_COLOR tan = al_color_name("tan");
      const ALLEGRO_COLOR teal = al_color_name("teal");
      const ALLEGRO_COLOR thistle = al_color_name("thistle");
      const ALLEGRO_COLOR tomato = al_color_name("tomato");
      const ALLEGRO_COLOR turquoise = al_color_name("turquoise");
      const ALLEGRO_COLOR violet = al_color_name("violet");
      const ALLEGRO_COLOR wheat = al_color_name("wheat");
      const ALLEGRO_COLOR white = al_color_name("white");
      const ALLEGRO_COLOR whitesmoke = al_color_name("whitesmoke");
      const ALLEGRO_COLOR yellow = al_color_name("yellow");
      const ALLEGRO_COLOR yellowgreen = al_color_name("yellowgreen");




   } // end namespace color
}




