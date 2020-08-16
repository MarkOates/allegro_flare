

#include <AllegroFlare/Profiler.hpp>

#include <iostream>
#include <sstream>
#include <algorithm>


namespace AllegroFlare
{
   Profiler::Profiler()
      : timers()
   {}


   Profiler::~Profiler()
   {
   }


   AllegroFlare::Timer *Profiler::find_or_create(std::string name)
   {
      std::map<std::string, AllegroFlare::Timer>::iterator it = timers.find(name);
      if (it == timers.end())
      {
         timers[name] = AllegroFlare::Timer();
         return &timers[name];
      }
      return &it->second;
   }


   std::map<std::string, AllegroFlare::Timer> Profiler::get_timers()
   {
      return this->timers;
   }


   std::map<std::string, std::vector<std::chrono::high_resolution_clock::time_point>> Profiler::get_event_buckets()
   {
      return this->event_buckets;
   }


   std::list<std::string> Profiler::get_event_bucket_names()
   {
      std::list<std::string> result;
      for (auto &event_bucket : event_buckets)
      {
         result.push_back(event_bucket.first);
      }
      return result;
   }


   int Profiler::get_event_bucket_num_samples(std::string event_bucket_name)
   {
      if (event_buckets.find(event_bucket_name) == event_buckets.end()) return 0;
      return event_buckets[event_bucket_name].size();
   }


   int Profiler::get_event_bucket_duration_between_last_two_samples(std::string event_bucket_name)
   {
      if (event_buckets.find(event_bucket_name) == event_buckets.end()) return 0;

      std::vector<std::chrono::high_resolution_clock::time_point> &bucket = event_buckets[event_bucket_name];
      if (bucket.size() <= 1) return 0;

      std::chrono::high_resolution_clock::time_point &most_recent_event_time = bucket[bucket.size() - 1];
      std::chrono::high_resolution_clock::time_point &second_most_recent_event_time = bucket[bucket.size() - 2];

      return std::chrono::duration_cast<std::chrono::microseconds>(
            most_recent_event_time - second_most_recent_event_time
         ).count();
   }


   void Profiler::start(std::string name)
   {
      find_or_create(name)->start();
   }


   void Profiler::pause(std::string name)
   {
      find_or_create(name)->pause();
   }


   void Profiler::reset(std::string name)
   {
      find_or_create(name)->reset();
   }


   void Profiler::emit(std::string name)
   {
      std::chrono::high_resolution_clock::time_point event_time = std::chrono::high_resolution_clock::now();
      event_buckets[name].push_back(event_time);
   }


   std::vector<std::chrono::high_resolution_clock::time_point> Profiler::get_event_samples(
      std::string name,
      int max_samples
   )
   {
      std::vector<std::chrono::high_resolution_clock::time_point> &events_for_this_bucket = event_buckets[name];
      if (max_samples < 0) max_samples = 0;
      if (max_samples > events_for_this_bucket.size()) max_samples = events_for_this_bucket.size();

      std::vector<std::chrono::high_resolution_clock::time_point> events_to_return
         (events_for_this_bucket.end() - max_samples, events_for_this_bucket.end());
      return events_to_return;
   }


   void Profiler::clear()
   {
      timers.clear();
      event_buckets.clear();
   }


#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
   void Profiler::draw(float x, float y, ALLEGRO_FONT *font)
   {
      if (!font) throw std::runtime_error("cannot Profiler::draw with nullptr font");

      //instance = new Profiler;
      ALLEGRO_COLOR bg_color = al_color_name("black");
      ALLEGRO_COLOR bar_color = al_color_name("orange");
      ALLEGRO_COLOR font_color = al_color_name("white");
      ALLEGRO_COLOR limit_bar_color = al_color_name("lightblue");
      //ALLEGRO_COLOR font = al_create_builtin_font();

      float w = 300;
      float line_height = 25;
      float pad = 20;
      float h = timers.size()*line_height + pad*2;
      //fla duration = 0.0;
      int i=0;
      char time[32];

      float target_microseconds = 16666;
      float horizontal_scale = 0.01f;

      al_draw_filled_rounded_rectangle(x, y, x+w, y+h, 8, 8, bg_color);

      //if (!font) return;

      //std::vector<profile_timer_class *>::iterator it = Profiler::get_instance()->timer.begin();
      for (auto &timer : timers)
      {
         int duration_microseconds = std::get<1>(timer).get_elapsed_time_microseconds();
         float bar_width = duration_microseconds * horizontal_scale;
         al_draw_filled_rectangle(
            x,
            y+pad+line_height*i+15,
            x+bar_width,
            y+pad+line_height*i+20,
            bar_color
         );

         al_draw_text(font, font_color, x+pad, y+pad+line_height*i, 0, timer.first.c_str());
         sprintf(time, "%d", duration_microseconds);
         al_draw_text(font, font_color, x+pad+300, y+pad+line_height*i, ALLEGRO_ALIGN_RIGHT, time);
         i++;
      }

      al_draw_line(
         target_microseconds * horizontal_scale,
         0,
         target_microseconds * horizontal_scale,
         h,
         ALLEGRO_COLOR{0, 1, 1, 1},
         2.0
      );
   }
}



