#include <bb_app.hpp>
#include <bb_assert.h>
#include <bb_defines.h>
#include <bb_logging.h>
#include <bb_string.h>
#include <imgui/imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <stb/stb_image.h>
#define GLCOREARB_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#include <khronos/GL/glcorearb.h>


typedef struct 
{
	GLuint texture_id;
	GLuint texture_width;
	GLuint texture_height;
} bb_texture_info;


typedef struct
{
	bb_texture_info* p_texture_infos;
	uint             num_texture_infos;
	ImFont*          p_font_13;
	ImFont*          p_font_20;
	ImFont*          p_font_32;
	ImFont*          p_font_64;
	ImVec2           child_size;
	ImVec2           flag_size;
	char             target_score_str[8];
	float            foul_button_x_pos;
	float            foul_button_y_pos;
} bb_gui_data;

static void bb_gamestate_init  (bb_game_state*);
static void bb_gamestate_deinit(bb_game_state*);
static void bb_player_init  (bb_player*);
static void bb_player_deinit(bb_player*);
static void bb_load_flag_icons(const char* folder, bb_gui_data* p_data);
static void bb_load_fonts     (const char* path,   bb_gui_data* p_data);

static void bb_run_setup(bb_game_state*, bb_gui_data*);
static void bb_run_break(bb_game_state*, bb_gui_data*);
static void bb_run_play (bb_game_state*, bb_gui_data*);
static int  bb_run_stats(bb_game_state*, bb_gui_data*);

static bb_texture_info bb_load_texture_from_file(const char* path);


void bb_app_run(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		BB_LOG_ERROR("%s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window*     window       = SDL_CreateWindow("Straight Pool", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext   gl_context   = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bb_game_state state;
	bb_gamestate_init(&state);

	bb_gui_data gui_data;
	BB_MEMSET(&gui_data, 0, sizeof(bb_gui_data));
	bb_load_flag_icons("flags", &gui_data);
	//bb_load_fonts("thirdparty/imgui/misc/fonts/DroidSans.ttf", &gui_data);
	bb_load_fonts("fonts/font.ttf", &gui_data);

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	int window_width  = 1280;
	int window_height = 720;
	int pane_width    = ((window_width / 2) - ImGui::GetStyle().WindowPadding.x - (2 * ImGui::GetStyle().FramePadding.x));
	int flag_width    = 128;
	BB_ASSERT(pane_width > flag_width, "Player pane is too small");
	gui_data.child_size.x      = (float)pane_width;
	gui_data.child_size.y      = 400.f;
	gui_data.flag_size.x       = (float)flag_width;
	gui_data.flag_size.y       = (float)flag_width;
	gui_data.foul_button_x_pos = 160.f;
	gui_data.foul_button_y_pos = 580.f;

	int done = 0;
	while(0 == done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_Always);

		switch(state.game_stage)
		{
			case BB_GAME_STAGE_SETUP:
				bb_run_setup(&state, &gui_data);
				break;
			case BB_GAME_STAGE_BREAK:
				bb_run_break(&state, &gui_data);
				break;
			case BB_GAME_STAGE_PLAY:
				bb_run_play(&state, &gui_data);
				break;
			case BB_GAME_STAGE_STATS:
				done |= bb_run_stats(&state, &gui_data);
				break;
		}

		ImGuiIO* p_io = &ImGui::GetIO();
		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)p_io->DisplaySize.x, (int)p_io->DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	/* destroy window */

	bb_gamestate_deinit(&state);
}


void bb_gamestate_init(bb_game_state* p_state)
{
	BB_MEMSET(p_state, 0, sizeof(bb_game_state));
	p_state->target_score       = 75u;
	p_state->inning_start_balls = 15u;
	p_state->balls_on_table     = 15u;
	p_state->current_player     = BB_PLAYER_1;
	p_state->game_stage         = BB_GAME_STAGE_SETUP;

	for (uint i = 0; i < BB_NUM_PLAYERS; ++i)
	{
		bb_player_init(&p_state->players[i]);
	}
}


void bb_gamestate_deinit(bb_game_state* p_state)
{
	for (uint i = 0; i < BB_NUM_PLAYERS; ++i)
	{
		bb_player_deinit(&p_state->players[i]);
	}
	
	BB_MEMSET(p_state, 0, sizeof(bb_game_state));
}


void bb_player_init(bb_player* p_player)
{
	BB_MEMSET(p_player, 0, sizeof(bb_player));
	BB_SNPRINTF(p_player->name, sizeof(p_player->name), "%s", "Player name");
	p_player->highrun = -3; /* must be lower than the worst possible score for a single inning: break foul, -2 */
}


void bb_player_deinit(bb_player* p_player)
{
	BB_MEMSET(p_player, 0, sizeof(bb_player));
}


static void loc_add_player_widgets_setup(const char* labelId, bb_player* p_player, bb_gui_data* p_data, BB_CURRENT_PLAYER side)
{
	int nationaliy = p_player->nationality;

	bb_texture_info ti = p_data->p_texture_infos[nationaliy];
	if (side == BB_PLAYER_1)
	{
		if (ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size))
		{
			ImGui::OpenPopup("nationality_selection");
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(p_data->child_size.x - p_data->flag_size.x - (2*ImGui::GetStyle().ItemSpacing.x));
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name));
	}
	else
	{
		ImGui::SetNextItemWidth(p_data->child_size.x - p_data->flag_size.x - (2*ImGui::GetStyle().ItemSpacing.x) - ImGui::GetStyle().WindowPadding.x);
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name));
		ImGui::SameLine();
		if (ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size))
		{
			ImGui::OpenPopup("nationality_selection");
		}
	}

	if (ImGui::BeginPopup("nationality_selection"))
	{
		uint num_flags = p_data->num_texture_infos;
		for (uint y = 0, e = (num_flags + 2) / 3; y < e; ++y)
		{
			uint remaining = num_flags - (y*3);
			remaining      = (remaining > 3) ? 3 : remaining;
			for (uint x = 0; x < remaining; ++x)
			{
				uint flag_idx = (y*3) + x;
				BB_ASSERT(flag_idx < num_flags, "Flag index out of bounds: %u >= %u", flag_idx, num_flags);
				bb_texture_info ti = p_data->p_texture_infos[flag_idx];
				if (ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size))
				{
					p_player->nationality = flag_idx;
					ImGui::CloseCurrentPopup();
				}

				if (x < 2)
				{
					ImGui::SameLine();
				}
			}
		}
		ImGui::EndPopup();
	}
}



void bb_run_setup(bb_game_state* p_state, bb_gui_data* p_data)
{
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("Setup", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings);

	ImGui::PushFont(p_data->p_font_64);

	ImGui::BeginChild("Player 1", p_data->child_size, 1);
	{
		bb_player* p_player = &p_state->players[BB_PLAYER_1];
		loc_add_player_widgets_setup("##Player1name", p_player, p_data, BB_PLAYER_1);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Player 2", p_data->child_size, 1);
	{
		bb_player* p_player   = &p_state->players[BB_PLAYER_2];
		loc_add_player_widgets_setup("##Player2name", p_player, p_data, BB_PLAYER_2);
	}
	ImGui::EndChild();

	ImGui::PopFont();
	ImGui::PushFont(p_data->p_font_32);

	ImGui::Text("Initial Break");
	ImGui::Text("Player 1"); ImGui::SameLine(); 
	ImGui::RadioButton("##break_radio_1", (int*)&p_state->current_player, BB_PLAYER_1); ImGui::SameLine();
	ImGui::RadioButton("##break_radio_2", (int*)&p_state->current_player, BB_PLAYER_2); ImGui::SameLine(); ImGui::Text("Player 2");

	int step      = 1;
	int step_fast = 10;
	if (ImGui::InputInt("Target Score", (int*)&p_state->target_score, step, step_fast))
	{
		/* disallow negative target score, reset to a sane low value */
		int score = *((int*)&p_state->target_score);
		if (score < 0)
		{
			p_state->target_score = 50;
		}
	}

	ImGui::SetCursorPosY(p_data->foul_button_y_pos);
	if (ImGui::Button("OK"))
	{
		p_state->game_stage = BB_GAME_STAGE_BREAK;
		BB_SPRINTF(p_data->target_score_str, "%4u", p_state->target_score);
	}
	ImGui::PopFont();
	ImGui::End();
}


void bb_run_break(bb_game_state* p_state, bb_gui_data* p_data)
{
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("Break", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings);

	ImGui::PushFont(p_data->p_font_64);

	bb_player* p_player   = &p_state->players[p_state->current_player];
	int        nationaliy = p_player->nationality;

	bb_texture_info ti = p_data->p_texture_infos[nationaliy];
	ImGui::BeginChild("Break", p_data->child_size, 1);
	ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size);
	ImGui::SameLine();
	ImGui::InputText("##Playername", p_player->name, sizeof(p_player->name), ImGuiInputTextFlags_ReadOnly);
	ImGui::PushFont(p_data->p_font_20);
	ImGui::PushFont(p_data->p_font_64);
	ImGui::Text("Score: %3d", p_player->score);
	ImGui::PopFont();
	ImGui::Text("Break: %3u", p_player->num_consecutive_fouls + 1u);
	ImGui::PopFont(); // 20
	ImGui::EndChild();

	ImGui::PushFont(p_data->p_font_32);
	ImGui::Text("%s", "");
	ImGui::Text("%s", "Balls");

	uint tmp_balls_on_table = p_state->balls_on_table;
	if (ImGui::InputInt("##balls_on_table", (int*)(&tmp_balls_on_table)))
	{
		/* prevent setting count to 0 or less: 0 balls is not a legal state in straight pool, less than 0 balls is impossible */
		if (*((int*)&tmp_balls_on_table) > 0)
		{
			/* don't allow manually going above 15 balls; the only way we go above 15 is by reracking when one ball is left, there's a button for that */
			if (tmp_balls_on_table <= 15)
			{
				p_state->balls_on_table = tmp_balls_on_table;
			}
		}
	}

	ImGui::SetCursorPosY(p_data->foul_button_y_pos);
	/* In straight pool, even the break shot has to be called. If the shooter doesn't
	 * make the ball they called, or if a safety was called, 2 object balls and the
	 * cue ball must be driven to a wall, otherwise the shot is a foul.
	 * In case of a foul, the opponent has the option to accept the table as is
	 * or to force the offending player to rebreak; rebreak means no state change,
	 * accept will always change the game stage and can also change the current player,
	 * if a called ball was pocketed (i.e. remaining balls < 15).
	 * nb: we rely on players to know the rules and spot illegally pocketed balls
	 *     before inputting numbers
	 */
	if (ImGui::Button("Accept"))
	{
		int num_balls_pocketed           = 15 - p_state->balls_on_table;
		int is_dry                       = num_balls_pocketed == 0;
		p_state->current_player          = ((p_state->current_player + is_dry) % BB_NUM_PLAYERS);
		p_state->game_stage              = BB_GAME_STAGE_PLAY;
		p_player->num_consecutive_fouls  = 0u;
		p_player->num_innings           += (int)(num_balls_pocketed == 0);
		BB_ASSERT(p_state->inning_start_balls == 15, "Should always be 15 balls when breaking");
	}
	ImGui::SameLine(p_data->foul_button_x_pos);
	if (ImGui::Button("Foul"))
	{
		/* break fouls don't count towards the 3 consecutive fouls that carry an 
		 * additional penalty, but they cost 2 points per instance instead of just 1.
		 */
		p_player->score                 -= 2;
		uint num_fouls                   = p_player->num_consecutive_fouls + 1;
		p_player->num_consecutive_fouls  = num_fouls; /* used to track and display the number of breaks */
		p_player->num_fouls             += 1;
		if (p_player->highrun < (((int)num_fouls) * -2))
		{
			p_player->highrun = ((int)num_fouls * -2);
		}
	}

	ImGui::PopFont(); // 32
	//ImGui::PopFont(); // 20
	ImGui::PopFont(); // 64

	ImGui::End();
}


static void loc_add_player_widgets_play(const char* labelId, bb_player* p_player, bb_gui_data* p_data, int is_active_player, BB_CURRENT_PLAYER side)
{
	int nationaliy = p_player->nationality;

	bb_texture_info ti = p_data->p_texture_infos[nationaliy];
	if (side == BB_PLAYER_1)
	{
		ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size);
		ImGui::SameLine();
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine(p_data->child_size.x - 48.f);
		ImGui::PushFont(p_data->p_font_32);
		ImGui::RadioButton("##player_active", 0 != is_active_player);
		ImGui::PopFont();
		ImGui::PushFont(p_data->p_font_20);
		ImGui::PushFont(p_data->p_font_64);
		ImGui::Text("Score: %3d", p_player->score);
		ImGui::PopFont();
		ImGui::Text("Fouls: %3u", p_player->num_consecutive_fouls);
		ImGui::PopFont();
	}
	else
	{
		ImGui::PushFont(p_data->p_font_32);
		ImGui::RadioButton("##player_active", 0 != is_active_player);
		ImGui::PopFont();
		ImGui::SameLine(64.f);
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size);
		ImGui::PushFont(p_data->p_font_20);
		ImGui::PushFont(p_data->p_font_64);
		ImGui::Text("Score: %3d", p_player->score);
		ImGui::PopFont();
		ImGui::Text("Fouls: %3u", p_player->num_consecutive_fouls);
		ImGui::PopFont();
	}

	ImGui::PushFont(p_data->p_font_64);
	if ((0 != is_active_player) && (p_player->num_consecutive_fouls == 2))
	{
		ImColor colorRed = ImColor(1.0f,0.4f,0.4f,1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)colorRed);
		ImGui::Text("%s", "2 FOUL WARNING");
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::Text("%s", "");
	}
	ImGui::PopFont();
}


void bb_run_play(bb_game_state* p_state, bb_gui_data* p_data)
{
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("##Play", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings);

	uint current_player_idx = p_state->current_player;

	ImGui::PushFont(p_data->p_font_64);

	ImGui::BeginChild("Player 1", p_data->child_size, 1);
	{
		bb_player* p_player = &p_state->players[BB_PLAYER_1];
		loc_add_player_widgets_play("##Player1name", p_player, p_data, current_player_idx == BB_PLAYER_1, BB_PLAYER_1);
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Player 2", p_data->child_size, 1);
	{
		bb_player* p_player = &p_state->players[BB_PLAYER_2];
		loc_add_player_widgets_play("##Player2name", p_player, p_data, current_player_idx == BB_PLAYER_2, BB_PLAYER_2);
	}
	ImGui::EndChild();

	bb_player* p_player = &p_state->players[current_player_idx];

	ImGui::PushFont(p_data->p_font_32);
	int step            = 1;
	int step_fast       = 10;
	uint inning_balls   = p_state->inning_start_balls;
	uint balls_on_table = p_state->balls_on_table;
	ImGui::Text("Inning Balls: %4u", inning_balls); ImGui::SameLine(p_data->child_size.x - (2 * p_data->flag_size.x));
	ImGui::Text("Target Score: %s", p_data->target_score_str); ImGui::SameLine(p_data->child_size.x + ImGui::GetStyle().ItemSpacing.x + (2 * ImGui::GetStyle().FramePadding.x));

	ImGui::SetNextItemWidth(196.f);
	if (ImGui::InputInt("Balls On Table", (int*)(&balls_on_table), step, step_fast))
	{
		/* prevent setting count to 0 or less: 0 balls is not a legal state in straight pool, less than 0 balls is impossible */
		if (*((int*)&balls_on_table) > 0)
		{
			if (balls_on_table <= 15)
			{
				p_state->balls_on_table = balls_on_table;
			}
		}
	}
	if (ImGui::Button("+14"))
	{
		p_state ->inning_start_balls    += 14;
		p_state ->balls_on_table         = 15u;
		p_player->num_consecutive_fouls  = 0u; /* a legal shot is implied */
	}

	/* add vertical space to prevent accidentally ending the inning */
	ImGui::SetCursorPosY(p_data->foul_button_y_pos);
	uint num_balls_pocketed = inning_balls - balls_on_table;
	if (ImGui::Button("End Inning"))
	{
		p_player->num_consecutive_fouls  = 0u; /* inning ends without a foul, consecutive foul count has to be reset */
		p_player->score                 += num_balls_pocketed;
		p_player->num_innings           += 1;
		p_state->current_player          = ((current_player_idx + 1) % BB_NUM_PLAYERS); /* change active player */
		int highrun                      = p_player->highrun;
		if (*((int*)&num_balls_pocketed) > highrun)
		{
			p_player->highrun = *((int*)&num_balls_pocketed);
		}
		p_state->inning_start_balls = p_state->balls_on_table;
		if (p_player->score >= *((int*)&p_state->target_score))
		{
			p_state->game_stage = BB_GAME_STAGE_STATS;
		}
	}
	ImGui::SameLine(p_data->foul_button_x_pos);
	if (ImGui::Button("Foul"))
	{
		/* if the inning ends because of a foul and no balls were legally pocketet, that means that the 1st shot in this inning was a foul, i.e. it counts towards the consecutive fouls;
		 * if there was a legal shot in between, the number of consecutive fouls had been set to 0, the new value will be 1.
		 */
		uint old_consecutive   = p_player->num_consecutive_fouls;
		uint new_consecutive   = ((old_consecutive + (num_balls_pocketed == 0)) % 3);
		uint third_consecutive = ((0 == new_consecutive) && (2 == old_consecutive));
		int  penalty           = 1 + (third_consecutive * 15); /* 3rd foul in a row carries an additional 15 point penalty */
		p_player->num_consecutive_fouls  = new_consecutive;
		p_player->score                 += (num_balls_pocketed - penalty);
		p_player->num_fouls             += 1;

		if (0 != third_consecutive)
		{
			/* rerack all 15 balls on 3rd consecutive foul and go back to the breaking stage */
			p_state->inning_start_balls = 15u; /* Any balls pocketed don't count, we can safely discard the previous number */
			p_state->balls_on_table     = 15u;
			p_state->game_stage         = BB_GAME_STAGE_BREAK;
		}
		else
		{
			int highrun = p_player->highrun;
			if ((int)(num_balls_pocketed - 1) > highrun)
			{
				p_player->highrun = (num_balls_pocketed - 1);
			}
			p_player->num_innings += 1;
			/* not the 3rd foul in a row, next player's inning starts */
			p_state->current_player     = ((current_player_idx + 1) % BB_NUM_PLAYERS);
			p_state->inning_start_balls = p_state->balls_on_table;
		}
	}
	ImGui::PopFont(); // 32
	ImGui::PopFont(); // 64
	ImGui::End();
}


static void loc_add_player_widgets_results_statistics(bb_gui_data* p_data, bb_player* p_player, float indent, float score_col)
{
	ImGui::PushFont(p_data->p_font_20);
	//if (indent > 0.f)
	{
		ImGui::Indent(indent);
	}
	ImGui::Text("Score:");  ImGui::SameLine(score_col); ImGui::Text("%3d", p_player->score);
	ImGui::Text("Fouls:");  ImGui::SameLine(score_col); ImGui::Text("%3u", p_player->num_consecutive_fouls);
	ImGui::Text("Innings:");ImGui::SameLine(score_col); ImGui::Text("%3u", p_player->num_innings);
	if (p_player->num_innings > 0)
	{
		ImGui::Text("Avg. balls:");  ImGui::SameLine(score_col); ImGui::Text("%3.2f", ((p_player->score * 1.f) / p_player->num_innings));
		ImGui::Text("High run:");    ImGui::SameLine(score_col); ImGui::Text("%3u", p_player->highrun);
	}
	else
	{
		ImGui::Text("Avg. balls:");  ImGui::SameLine(score_col); ImGui::Text("%s", "N/A");
		ImGui::Text("High run:");    ImGui::SameLine(score_col); ImGui::Text("%s", "N/A");
	}
	//if (indent > 0.f)
	{
		ImGui::Unindent();
	}
	ImGui::PopFont();
}


static void loc_add_player_widgets_result(const char* labelId, bb_player* p_player, bb_gui_data* p_data, int is_active_player, int is_winner, BB_CURRENT_PLAYER side)
{
	int nationaliy = p_player->nationality;

	bb_texture_info ti = p_data->p_texture_infos[nationaliy];
	if (side == BB_PLAYER_1)
	{
		ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size);
		ImGui::SameLine();
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name), ImGuiInputTextFlags_ReadOnly);
		loc_add_player_widgets_results_statistics(p_data, p_player, 0.f, 128.f);
	}
	else
	{
		ImGui::Indent(64.f);
		ImGui::InputText(labelId, p_player->name, sizeof(p_player->name), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		ImGui::ImageButton((void*)(intptr_t)ti.texture_id, p_data->flag_size);
		loc_add_player_widgets_results_statistics(p_data, p_player, 2 * p_data->flag_size.x, p_data->child_size.x - p_data->flag_size.x);
	}

	ImGui::PushFont(p_data->p_font_64);
	if (0 != is_winner)
	{
		ImColor color_green = ImColor(0.0f,1.0f,0.0f,1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)color_green);
		ImGui::Text("%s", "WINNER");
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::Text("%s", "");
	}
	ImGui::PopFont();
}


int bb_run_stats(bb_game_state* p_state, bb_gui_data* p_data)
{
	int quit_requested = 0;
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("##Result", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings);

	uint current_player_idx = p_state->current_player;

	ImGui::PushFont(p_data->p_font_64);

	ImGui::BeginChild("Player 1", p_data->child_size, 1);
	{
		bb_player* p_player = &p_state->players[BB_PLAYER_1];
		loc_add_player_widgets_result("##Player1name", p_player, p_data, current_player_idx == BB_PLAYER_1, p_player->score >= *((int*)&p_state->target_score), BB_PLAYER_1);
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Player 2", p_data->child_size, 1);
	{
		bb_player* p_player = &p_state->players[BB_PLAYER_2];
		loc_add_player_widgets_result("##Player2name", p_player, p_data, current_player_idx == BB_PLAYER_2, p_player->score >= *((int*)&p_state->target_score), BB_PLAYER_2);
	}
	ImGui::EndChild();

	ImGui::PushFont(p_data->p_font_32);

	ImGui::SetCursorPosY(p_data->foul_button_y_pos);

	if (ImGui::Button("New Game"))
	{
		bb_gamestate_init(p_state);
		p_state->game_stage = BB_GAME_STAGE_SETUP;
	}
	ImGui::SameLine(p_data->foul_button_x_pos);
	if (ImGui::Button("Quit"))
	{
		quit_requested =1 ;
	}
	ImGui::PopFont(); // 32
	ImGui::PopFont(); // 64
	ImGui::End();

	return quit_requested;
}


bb_texture_info bb_load_texture_from_file(const char* path)
{
	bb_texture_info result;

	unsigned char* image_data = stbi_load(path, (int*)&result.texture_width, (int*)&result.texture_height, NULL, 4);
	if (image_data == NULL)
	{
		BB_MEMSET(&result, 0, sizeof(bb_texture_info));
		return result;
	}

	// Create a OpenGL texture identifier
	glGenTextures(1, &result.texture_id);
	glBindTexture(GL_TEXTURE_2D, result.texture_id);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.texture_width, result.texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	return result;
}


extern "C" {
extern uint bb_get_filenames_in_folder(const char* foldername, char*** p_names_array_out);
}

void bb_load_flag_icons(const char* folder, bb_gui_data* p_data)
{
	char** icon_filenames = NULL;
	uint num_files        = bb_get_filenames_in_folder(folder, &icon_filenames);

	p_data->p_texture_infos   = (bb_texture_info*)malloc(num_files * sizeof(bb_texture_info));
	p_data->num_texture_infos = num_files;

	glEnable(GL_TEXTURE_2D);
	for (uint i = 0; i < num_files; ++i)
	{
		p_data->p_texture_infos[i] = bb_load_texture_from_file(icon_filenames[i]);
	}
	free(icon_filenames);
}


void bb_load_fonts(const char* path, bb_gui_data* p_data)
{
	ImGuiIO*     p_io    = &ImGui::GetIO();
	ImFontAtlas* p_atlas = p_io->Fonts;
	ImFont*      p_font = NULL;
	p_font = p_atlas->AddFontFromFileTTF(path, 13.f);
	BB_ASSERT(NULL != p_font, "Failed to load font from ttf %s with size 13", path);
	p_data->p_font_13 = p_font;
	p_font = p_atlas->AddFontFromFileTTF(path, 20.f);
	BB_ASSERT(NULL != p_font, "Failed to load font from ttf %s with size 20", path);
	p_data->p_font_20 = p_font;
	p_font = p_atlas->AddFontFromFileTTF(path, 32.f);
	BB_ASSERT(NULL != p_font, "Failed to load font from ttf %s with size 32", path);
	p_data->p_font_32 = p_font;
	p_font = p_atlas->AddFontFromFileTTF(path, 64.f);
	BB_ASSERT(NULL != p_font, "Failed to load font from ttf %s with size 64", path);
	p_data->p_font_64 = p_font;
}
