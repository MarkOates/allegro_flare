#pragma once




#include <allegro_flare/surface_area_base.h>




namespace allegro_flare
{
   class UISurfaceAreaNeverCollide : public UISurfaceAreaBase
   {
   public:
      UISurfaceAreaNeverCollide(float x, float y, float w, float h);
      ~UISurfaceAreaNeverCollide();

      void draw_bounding_area();
      bool collides(float x, float y);
   };
}



