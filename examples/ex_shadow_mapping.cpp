

#include <allegro_flare/allegro_flare.h>
#include <allegro5/allegro_opengl.h>

#include <allegro_flare/cubemap.h>



using namespace allegro_flare;
using namespace AllegroFlare;



class Camera3
{
public:
	vec3d position;
	vec3d stepout;
	float spin;
	float tilt;
	Camera3()
		: position(0, 0, 0)
		, stepout(0, 0, 0)
		, spin(0)
		, tilt(0)
	{}
	void position_transform(ALLEGRO_TRANSFORM *t)
	{
		al_identity_transform(t);

		al_translate_transform_3d(t, stepout.x, stepout.y, stepout.z);
		al_rotate_transform_3d(t, -1, 0, 0, tilt);
		al_rotate_transform_3d(t, 0, -1, 0, spin);
	}
	void reverse_position_transform(ALLEGRO_TRANSFORM *t)
	{
		// note: this is EXACTLY the same as position transform, except the
		// order of transformations is reversed, and the values are negated
		al_identity_transform(t);

		al_rotate_transform_3d(t, 0, 1, 0, spin);
		al_rotate_transform_3d(t, 1, 0, 0, tilt);
		al_translate_transform_3d(t, -stepout.x, -stepout.y, -stepout.z);
	}
	vec3d get_real_position()
	{
		vec3d real_position(0, 0, 0);
		ALLEGRO_TRANSFORM t;

		position_transform(&t);
		al_transform_coordinates_3d(&t, &real_position.x, &real_position.y, &real_position.z);

		return real_position;
	}
};




class Light
{
public:
	vec3d position;
	Light(float x, float y, float z)
		: position(x, y, z)
	{}
};





class Entity
{
private:
	static Shader *standard_compound_shader;
	static Shader *__get_standard_compound_shader()
	{
		if (standard_compound_shader) return standard_compound_shader;

		std::cout << "Building Entity::standard_compound_shader." << std::endl;
		standard_compound_shader = new Shader("../data/shaders/standard_compound_vertex.glsl", "../data/shaders/standard_compound_fragment.glsl");
		if (!standard_compound_shader)
		{
			std::cout << "There was a problem creating the compound shader for Entity" << std::endl;
		}
		return standard_compound_shader;
	}

	static int last_id;
  int id;
public:
  placement3d place;
  placement3d velocity;
  Model3D *model;
  bool shader_applies_transform;


  // Eventually, there could be n shaders for each model
  // for example, one for each named object.  Initially,
  // however, we'll stick with one shader and texture set
  // per entity.
  Shader *shader;
  ALLEGRO_BITMAP *diffuse_texture;
  ALLEGRO_BITMAP *normal_texture;
  ALLEGRO_BITMAP *specular_texture;
  ALLEGRO_FLARE_CUBEMAP_TEXTURE *cube_map_A;
  ALLEGRO_FLARE_CUBEMAP_TEXTURE *cube_map_B;
  bool cube_map_reflecting;

  Entity()
	  : id(last_id)
	  , model(NULL)
	  , shader(__get_standard_compound_shader())
	  , place()
	  , velocity()
	  , shader_applies_transform(true)
	  , diffuse_texture(NULL)
	  , normal_texture(NULL)
	  , specular_texture(NULL)
	  , cube_map_A(NULL)
	  , cube_map_B(NULL)
	  , cube_map_reflecting(true)
  {}

  void draw(vec3d camera_position, vec3d light_position, Shader *override_shader=NULL, ALLEGRO_TRANSFORM *shadow_map_depth_pass_transform=NULL, ALLEGRO_BITMAP *shadow_map_depth_pass_surface=NULL, vec2d texture_offset=0)
  {
		ALLEGRO_STATE previous_state;
		ALLEGRO_TRANSFORM transform;

		if (override_shader)
		{
			// these ones are not needed
			{
				Shader::set_vec3("light_position", light_position);
		   }

			Shader::set_bool("reflecting", cube_map_reflecting);

			Shader::set_sampler("diffuse_texture", diffuse_texture, 2);
			Shader::set_sampler("specular_texture", specular_texture, 3);
			Shader::set_sampler("normal_texture", normal_texture, 4);
         Shader::set_sampler_cube("cube_map_A", cube_map_A, 3);
			//Shader::set_sampler_cube("cube_map_B", cube_map_B, 4); // <- this crashes in some cases
			place.build_transform(&transform);
			Shader::set_vec3("camera_position", camera_position);
			Shader::set_mat4("position_transform", &transform);

			if (model) model->draw();
			return;
		}
		


		if (shader && shader_applies_transform)
		{
			// construct our entity's transform
			place.build_transform(&transform);
		  //std::cout << "." << std::endl; 


			// Now apply it to the shader
			shader->use();
			Shader::set_vec3("camera_position", camera_position);
			Shader::set_vec3("light_position", light_position);
			Shader::set_mat4("position_transform", &transform);

			Shader::set_bool("reflecting", cube_map_reflecting);

			Shader::set_sampler("diffuse_texture", diffuse_texture, 0);

			if (shadow_map_depth_pass_transform && shadow_map_depth_pass_surface)
			{
				Shader::set_sampler("depth_pass_surface", shadow_map_depth_pass_surface, 1);
				Shader::set_mat4("depth_pass_transform", shadow_map_depth_pass_transform);
				Shader::set_float("texture_offset_x", texture_offset.x);
				Shader::set_float("texture_offset_y", texture_offset.y);
			}
			//Shader::set_sampler("specular_texture", specular_texture, 3);
			//Shader::set_sampler("normal_texture", normal_texture, 4);
			Shader::set_sampler_cube("cube_map_A", cube_map_A, 3);
			Shader::set_sampler_cube("cube_map_B", cube_map_B, 4);
			if (diffuse_texture) model->set_texture(diffuse_texture);
		}
		else
		{
		  std::cout << "." << std::endl; 
			// when not using the shader, we'll need to 
			// apply the transform directly here
			al_store_state(&previous_state, ALLEGRO_STATE_TRANSFORM);
			al_identity_transform(&transform);
			al_use_transform(&transform);
			place.start_transform();

			// also, we set the texture on this model directly
			if (diffuse_texture) model->set_texture(diffuse_texture);
		}



		// actually draw our model here
		if (model) model->draw();



		if (shader && shader_applies_transform)
		{
			Shader::stop();
		}
		else
		{
			place.restore_transform();
			al_restore_state(&previous_state);
		}
  }
};
Shader *Entity::standard_compound_shader = NULL;
int Entity::last_id = 1;






class My3DProject : public Screen
{
public:
	// bins
	BitmapBin bitmaps;
	//ModelBin models;

	// assets
	Camera3 camera;
	Camera3 casting_light;
	//Model3D construct;
	//Model3D model;
	//Model3D *current_model;
	//Model3D unit_sphere;
	//Shader metalic_shader;
	//Shader fresnel_shader;
	Shader cubemap_shader;
	Shader depth_shader;
	Shader simple_map_shader;
	//Shader multi_shader;
	//Shader *current_shader;
	ALLEGRO_FLARE_CUBEMAP_TEXTURE *cube_map_A;
	ALLEGRO_FLARE_CUBEMAP_TEXTURE *cube_map_B;
	Light light;

  vec2d texture_offset;

  ALLEGRO_TRANSFORM shadow_map_depth_pass_transform;
  ALLEGRO_BITMAP *shadow_map_depth_pass_surface;

	std::vector<Entity *> entities;
   Entity *skybox;

	My3DProject(Display *display)
		: Screen(display)
		, bitmaps()
		, camera()
		, casting_light()
		//, metalic_shader("data/shaders/metalic_vertex.glsl", "data/shaders/metalic_fragment.glsl")
		//, fresnel_shader("data/shaders/fresnel_vertex.glsl", "data/shaders/fresnel_fragment.glsl")
		, cubemap_shader("../data/shaders/cube_vertex.glsl", "../data/shaders/cube_fragment.glsl")
		//, multi_shader("data/shaders/multi_vertex.glsl", "data/shaders/multi_fragment.glsl")
		, depth_shader("../data/shaders/depth_vertex.glsl", "../data/shaders/depth_fragment.glsl")
		, simple_map_shader("../data/shaders/simple_map_vertex_with_light.glsl", "../data/shaders/simple_map_fragment_with_light.glsl")
		, light(4, 4, 3)
		, cube_map_A(NULL)
		, cube_map_B(NULL)
		, skybox(NULL)
		, shadow_map_depth_pass_transform()
		, shadow_map_depth_pass_surface(NULL)
		, texture_offset(0, 0)
	{
      initialize();

		al_identity_transform(&shadow_map_depth_pass_transform);

		//camera.stepout = vec3d(0, 1.5, 8);
		camera.stepout = vec3d(0, 0, 10);
		//camera.tilt = 0.3;
		camera.tilt = 0.6;

		casting_light.stepout = vec3d(0, 0, 16);
		//casting_light.stepout = vec3d(0, 1, 33);
		//casting_light.stepout = vec3d(0, 5, 33);
		//casting_light.stepout = vec3d(0, 1.5, 8);
		casting_light.tilt = 0.8;

		cube_map_A = glsl_create_cubemap_from_vertical_strip("../data/bitmaps/sky4.png");
		cube_map_B = glsl_create_cubemap_from_vertical_strip("../data/bitmaps/sky5_with_grid.png");

		skybox = new Entity();
		skybox->model = new Model3D();
		skybox->model->load_obj_file("../data/models/skybox-01.obj", 30.0/3.0);
		skybox->shader = &cubemap_shader;
		skybox->cube_map_A = cube_map_B;
		skybox->cube_map_B = cube_map_B;
		skybox->cube_map_reflecting = false;

		shadow_map_depth_pass_surface = al_create_bitmap(display->width(), display->height());

		construct_scene();
	}


   void initialize()
   {
      bitmaps.set_path("../data/bitmaps");
   }


	void construct_scene()
	{
		Entity *entity = NULL;

		// add some nice models for us to look at
		Model3D *a_nice_model = new Model3D();
		Model3D *coin_ring = new Model3D();
		//a_nice_model->load_obj_file("data/models/allegro_flare_logo-03b.obj", 1.25);
		a_nice_model->load_obj_file("../data/models/rounded_unit_cube-01.obj", 1.0);
		coin_ring->load_obj_file("../data/models/coin_ring-01.obj", 1.0);

		for (unsigned i=0; i<10; i++)
		{
			entity = new Entity();
			entity->model = a_nice_model;
			entity->cube_map_A = cube_map_B;
			entity->cube_map_B = cube_map_B;
			entity->place.position = vec3d(random_float(-5, 5), random_float(0, 5), random_float(-5, 5));
			entity->place.rotation = vec3d(random_float(0, 1), random_float(0, 1), random_float(0, 1));
			entity->velocity.rotation = vec3d(random_float(0, 0.001), random_float(0, 0.001), random_float(0, 0.001));
			entity->shader = &simple_map_shader;
			//entity->diffuse_texture = bitmaps["rounded_unit_cube_uv-01.png"];//tex_grass_002.jpg"];
			//entity->diffuse_texture = bitmaps["tex_grass_002.jpg"];
			entity->diffuse_texture = bitmaps["uv.png"];
			entities.push_back(entity);
			if ((i%2) == 0)
			{
				entity->shader = &cubemap_shader;
				entity->model = coin_ring;
				entity->cube_map_A = cube_map_A;
				entity->cube_map_B = cube_map_A;
				entity->place.position = vec3d(random_float(-5, 5), 0.5, random_float(-5, 5));
				entity->place.rotation = vec3d(0, random_float(0, 1), 0);
				entity->velocity.rotation = vec3d(0, 0.003, 0);
			}
		}

		for (unsigned i=0; i<2; i++)
		{
			int r = i / 3;
			int c = i % 3;
			// add the Construct(s)
			entity = new Entity();
			entity->shader = &simple_map_shader;
			entity->model = new Model3D();
				entity->model->load_obj_file("../data/models/flat_stage-01.obj");
				//entity->model->load_obj_file("data/models/scene_for_lighting-03.obj");
				//entity->model->set_texture(bitmaps["uv.png"]);
				//entity->diffuse_texture = bitmaps["grass_grass_0118_02_preview.jpg"];//tex_grass_002.jpg"];
				entity->diffuse_texture = bitmaps["uv.png"];//tex_grass_002.jpg"];
				entity->place.position = vec3d(i*10, 0, 0);//tex_grass_002.jpg"];
				//entity->place.rotation = vec3d(0.01, 0, 0.02);//tex_grass_002.jpg"];
			entities.push_back(entity);
		}
	
		//archway
		Model3D *archway = new Model3D();
		archway->load_obj_file("../data/models/archway-01.obj", 0.5);
		Model3D *stone_fence = new Model3D();
		stone_fence->load_obj_file("../data/models/unit_stone_fence-02.obj", 2.0);
		for (int i=0; i<2; i++)
		{
			entity = new Entity();
			entity->shader = &simple_map_shader;
			entity->model = archway;
			entity->diffuse_texture = bitmaps["bricksz.jpg"];//tex_grass_002.jpg"];
			entity->place.position = vec3d(-5 + 10*i, 0, 0);//tex_grass_002.jpg"];
			entities.push_back(entity);

				// borders to the archway
				entity = new Entity();
				entity->shader = &simple_map_shader;
				entity->model = stone_fence;
				entity->diffuse_texture = bitmaps["bricksz.jpg"];//tex_grass_002.jpg"];
				entity->place.position = vec3d(-5 + 10*i, 0, -4);
				entity->place.rotation = vec3d(0, 0.25, 0);
				entities.push_back(entity);

				// borders to the archway
				entity = new Entity();
				entity->shader = &simple_map_shader;
				entity->model = stone_fence;
				entity->diffuse_texture = bitmaps["bricksz.jpg"];//tex_grass_002.jpg"];
				entity->place.position = vec3d(-5 + 10*i, 0, 4);
				entity->place.rotation = vec3d(0, 0.25, 0);
				entities.push_back(entity);
		}
	}

	void setup_projection_SHADOW(Camera3 &camera_to_use, ALLEGRO_TRANSFORM *transform_to_fill=NULL)
	{
		// setup the render settings
		al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
		al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_DEPTH | ALLEGRO_MASK_RGBA);
		al_clear_depth_buffer(1);


		ALLEGRO_TRANSFORM t;

		camera_to_use.reverse_position_transform(&t);
	
		float aspect_ratio = (float)al_get_bitmap_height(backbuffer_sub_bitmap) / al_get_bitmap_width(backbuffer_sub_bitmap);
	//	al_perspective_transform(&t, -1, aspect_ratio, 1, 1, -aspect_ratio, 100);
	//	al_perspective_transform(&t, -1, aspect_ratio, 4, 1, -aspect_ratio, 100);
		float w = al_get_bitmap_width(backbuffer_sub_bitmap);
//	   al_orthographic_transform(&t, -1*w, aspect_ratio*w, -100, -1*w, -aspect_ratio*w, 1000);
		ALLEGRO_BITMAP *bitmap = backbuffer_sub_bitmap;
//al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), -al_get_bitmap_height(bitmap), -100.0, al_get_bitmap_width(bitmap),
                          //al_get_bitmap_height(bitmap), 100.0);

		//al_scale_transform_3d(&t, al_display_width(bitmap), al_display_height(bitmap));
		al_scale_transform_3d(&t, 150, 150, 1); 
		
al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 30.0, al_get_bitmap_width(bitmap),
                          -al_get_bitmap_height(bitmap), -30.0);

		if (transform_to_fill != NULL)
		{
			al_copy_transform(transform_to_fill, &t);
		}

		//al_set_target_bitmap(backbuffer_sub_bitmap); << I don't think this is necessairy, it also occours just prior to this function
		al_use_projection_transform(&t);
	}

	void setup_projection_SCENE(Camera3 &camera_to_use, ALLEGRO_TRANSFORM *transform_to_fill=NULL)
	{
		// setup the render settings
		al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
		al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_DEPTH | ALLEGRO_MASK_RGBA);
		al_clear_depth_buffer(1);


		ALLEGRO_TRANSFORM t;

		camera_to_use.reverse_position_transform(&t);
	
		float aspect_ratio = (float)al_get_bitmap_height(backbuffer_sub_bitmap) / al_get_bitmap_width(backbuffer_sub_bitmap);
		al_perspective_transform(&t, -1, aspect_ratio, 1, 1, -aspect_ratio, 100);
	//	al_perspective_transform(&t, -1, aspect_ratio, 4, 1, -aspect_ratio, 100);
/*
		float w = al_get_bitmap_width(backbuffer_sub_bitmap);
//	   al_orthographic_transform(&t, -1*w, aspect_ratio*w, -100, -1*w, -aspect_ratio*w, 1000);
		ALLEGRO_BITMAP *bitmap = backbuffer_sub_bitmap;
//al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), -al_get_bitmap_height(bitmap), -100.0, al_get_bitmap_width(bitmap),
                          //al_get_bitmap_height(bitmap), 100.0);

		//al_scale_transform_3d(&t, al_display_width(bitmap), al_display_height(bitmap));
		al_scale_transform_3d(&t, 150, 150, 1); 
*/		
/*
 al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 30.0, al_get_bitmap_width(bitmap),
                          -al_get_bitmap_height(bitmap), -30.0);
*/
		if (transform_to_fill != NULL)
		{
			al_copy_transform(transform_to_fill, &t);
		}

		//al_set_target_bitmap(backbuffer_sub_bitmap); << I don't think this is necessairy, it also occours just prior to this function
		al_use_projection_transform(&t);
	}
	void draw_the_shadow_map_depth_pass()
	{
		al_clear_to_color(color::white);
		setup_projection_SHADOW(casting_light, &shadow_map_depth_pass_transform);

		// setup the lights
		vec3d camera_position = casting_light.get_real_position();
		vec3d light_position = light.position * 100.0;
		
		// setup the shader
		depth_shader.use();

		
		// draw the objects
		for (unsigned i=0; i<entities.size(); i++)
			entities[i]->draw(camera_position, light_position, &depth_shader);
		
	}
	void primary_timer_func() override
	{
		//
		// UPDATE ALL THE THINGS
		//
		casting_light.spin -= 1.0 / 60.0 * 0.05;
		light.position = casting_light.get_real_position();
//		camera.spin += 1.0 / 60.0 * 0.1;

		for (unsigned i=0; i<entities.size(); i++)
		{
			entities[i]->place.position += entities[i]->velocity.position;
			entities[i]->place.rotation += entities[i]->velocity.rotation;
		}



		//
		// DRAW THE DEPTH PASS
		//
		glEnable(GL_CULL_FACE); // requiring opengl should eventually be fazed out
		glCullFace(GL_FRONT); 

		al_set_target_bitmap(backbuffer_sub_bitmap);
		//al_set_target_bitmap(depth_pass_surface);
		//al_set_target_bitmap(depth_pass_surface);
		draw_the_shadow_map_depth_pass();

		al_set_target_bitmap(shadow_map_depth_pass_surface);
		al_draw_bitmap(backbuffer_sub_bitmap, 0, 0, 0);

		//glEnable(GL_CULL_FACE); // requiring opengl should eventually be fazed out
		glCullFace(GL_BACK); 
		//glCullFace(GL_FRONT);
		glDisable(GL_CULL_FACE);





		//
		// DRAW SCENE
		//
		
		// 1. create some frequently used variables
		vec3d camera_position = camera.get_real_position();
		vec3d light_position = light.position * 100.0;
		//vec3d light_position = light.position;
		

		// 2. set the target to the backbuffer
		al_set_target_bitmap(backbuffer_sub_bitmap);
		al_clear_to_color(color::slategray);
		setup_projection_SCENE(camera); // the functionality of this line might eventually be embedded in the framework


		// 3. draw the skybox (and clear the depth buffer)
		skybox->place.position = camera_position;
		skybox->draw(0, 0);
		al_clear_depth_buffer(1);

		
		// 4. draw all the entities
		for (unsigned i=0; i<entities.size(); i++)
		{
			entities[i]->draw(camera_position, light_position, NULL,
					&shadow_map_depth_pass_transform, shadow_map_depth_pass_surface, texture_offset);
		}


		// draw the Construct (the environment box)
/*
		glEnable(GL_CULL_FACE); // requiring opengl should eventually be fazed out
		construct.draw();
		glDisable(GL_CULL_FACE);
*/
		Shader::stop();

		static int count = 0;
		count++;
		if (count > 120)
		{
			//al_save_bitmap("depth_bitmap.png", depth_pass_surface);
			//al_save_bitmap("regular_bitmap.png", backbuffer_sub_bitmap);
			count = 0;
		}
	}
};




class ImageResizer : public UIImage
{
	bool zoomed;
	bool draw_outline;
	public:
		ImageResizer(UIWidget *parent, float x, float y, ALLEGRO_BITMAP *image)
			: UIImage(parent, x, y, image)
		   , zoomed(false)
		   , draw_outline(false)
		{
		}
		void toggle_zoom()
		{
		  float (*interp_func)(float) = interpolator::tripple_fast_in;
		  if (zoomed)
		  {
			  Framework::motion().cmove_to(&place.scale.x, 0.8, 0.4, interp_func);
			  Framework::motion().cmove_to(&place.scale.y, 0.8, 0.4, interp_func);
		  }
		  else
		  {
			  Framework::motion().cmove_to(&place.scale.x, 0.2, 0.4, interp_func);
			  Framework::motion().cmove_to(&place.scale.y, 0.2, 0.4, interp_func);
		  }
		  zoomed = !zoomed;
		}
		void on_click() override
		{
		  toggle_zoom();
		}
		void on_mouse_enter() override
		{
			draw_outline = true;
		}
		void on_mouse_leave() override
		{
			draw_outline = false;
		}
};



class MyGUIScreen : public UIScreen
{
private:
	My3DProject *scene;
	UIToggleButton *button;
	UIListSpinner *shader_choice_spinner;
	 UIDial *offset_x;
	 UIDial *offset_y;
	// state
	bool camera_spinning;
   ImageResizer *image;

public:
	MyGUIScreen(Display *display, My3DProject *scene)
		: UIScreen(display)
		, scene(scene)
		, button(new UIToggleButton(this, 110, display->height()-70, 160, 50, "camera spin"))
		, camera_spinning(false)
		, offset_x(new UIDial(this, 100, 100, 80))
		, offset_y(new UIDial(this, 100, 200, 80))
		, image(new ImageResizer(this, display->width()-10, 10, scene->shadow_map_depth_pass_surface))
	{
		button->toggle();
		image->place.scale = vec2d(0.2, 0.2);
		image->place.align = vec2d(1.0, 0.0);
	}
	void on_message(UIWidget *sender, std::string message) override
	{
		if (sender == button) camera_spinning = !camera_spinning;
		if (sender == offset_x) scene->texture_offset.x = offset_x->get_value();
		if (sender == offset_y) scene->texture_offset.y = offset_y->get_value();
		std::cout << offset_x->get_value() << "  " << offset_y->get_value() << std::endl;
	}
	void on_timer() override
	{
		// spin the camera
		if (camera_spinning) scene->camera.spin += 1.0 / 60.0 * 0.1;
	}
};




int main(int argc, char **argv)
{
	Framework::initialize();
	Display *display = Framework::create_display(1920, 1080, ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);
	My3DProject *proj = new My3DProject(display);
	MyGUIScreen *gui = new MyGUIScreen(display, proj);
	Framework::run_loop();
	return 0;
}



