#include <stdlib.h>

#include <tonc.h>

#include <general.h>

#include <data/BG_Menu.h>
#include <data/BG_MenuOverlay.h>










// 300 = 5s
#define RESET_TIMER 300










extern uint snake_score;
extern uint snake_score_high;
extern uint memory_raid_score;
extern uint memory_raid_score_high;










gameIndex_t init_menu(void)
{
	REG_DISPCNT = DCNT_MODE0;



	// Definindo os mapas
	REG_BG0CNT = BG_CBB(0) | BG_SBB(24)	| BG_REG_64x32 | BG_8BPP | BG_PRIO(1);
	REG_BG1CNT = BG_CBB(2) | BG_SBB(26)	| BG_REG_32x32 | BG_8BPP | BG_PRIO(0);

	// Copiando os tiles
	memcpy32(tile8_mem[0], BG_MenuTiles, BG_MenuTilesLen / 4);
	memcpy32(tile8_mem[2], BG_MenuOverlayTiles, BG_MenuOverlayTilesLen / 4);

	// Copiando os mapas
	memcpy32(se_mem[24], BG_MenuMap, BG_MenuMapLen / 4);
	memcpy32(se_mem[26], BG_MenuOverlayMap, BG_MenuOverlayMapLen / 4);

	// A paleta de cores é igual para os dois
	memcpy32(pal_bg_mem, BG_MenuPal, BG_MenuPalLen / 4);
	pal_bg_mem[0] = CLR_BLACK;





	// Toda vez que volta para o menu continua no mesmo lugar
	static int game_index = SNAKE_GAME;
	static int xoffset = 0;

	// Usado para reiniciar a pontuação mais alta
	int reset_timer = 0;





	// Guarda as strings das pontuações
	char scores_buffer[4][9] = {0};

	snprintf(scores_buffer[0], 9, "%8.8d", 100 * snake_score_high);
	snprintf(scores_buffer[1], 9, "%8.8d", 100 * snake_score);
	snprintf(scores_buffer[2], 9, "%8.8d", 100 * memory_raid_score_high);
	snprintf(scores_buffer[3], 9, "%8.8d", 100 * memory_raid_score);



	// Muito complicado para explicar, apenas inicia o texto para as pontuações
	tte_init_se(
        0,
        BG_CBB(0) | BG_SBB(24)	| BG_REG_64x32 | BG_8BPP | BG_PRIO(1),
        BG_MenuTilesLen / sizeof(TILE8),
        CLR_WHITE,
        14,
        NULL,
        NULL
	); 



	// Imprime as pontuações atuais e as maiores pontuações
	tte_set_pos(64, 64);
	tte_write(scores_buffer[0]);
	tte_set_pos(64, 72);
	tte_write(scores_buffer[1]);
	tte_set_pos(64, 64 + 256);
	tte_write(scores_buffer[2]);
	tte_set_pos(64, 72 + 256);
	tte_write(scores_buffer[3]);



	// Ativando os backgrounds
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;
	REG_BG0HOFS = xoffset;










	while (1)
	{
		key_poll();



		// Se o jogador segurar SELECT as pontuações são resetadas
		if (key_is_down(KEY_SELECT))
		{
			if (reset_timer > RESET_TIMER)
			{
				snake_score				= 0;
				snake_score_high		= 0;
				memory_raid_score		= 0;
				memory_raid_score_high	= 0;

				// Marca como não salvo ainda
				flash_save_word(0xFFFFFFFF, NULL_GAME);

				return NULL_GAME;
			}

			reset_timer++;
			VBlankIntrWait();
		}
		else
		{
			// O botão SELECT foi soltado e o timer reiniciou
			reset_timer = 0;
		}
		
		

		// Retorna para main e executa o jogo especificado
		if (key_hit(KEY_START))
		{
			return game_index;
		}




		// Move esquerda/direita
		if (key_is_down(KEY_R) || key_is_down(KEY_RIGHT))
		{
			for (int i = 0; i < 64; i++)
			{
				xoffset += 4;
				VBlankIntrWait();
				REG_BG0HOFS = xoffset;
			}

			game_index = (game_index + NUM_GAMES + 1) % NUM_GAMES;
		}
		else if (key_is_down(KEY_L) || key_is_down(KEY_LEFT))
		{
			for (int i = 0; i < 64; i++)
			{
				xoffset -= 4;
				VBlankIntrWait();
				REG_BG0HOFS = xoffset;
			}

			game_index = (game_index + NUM_GAMES - 1) % NUM_GAMES;
		}



		// Deixa um pouco mais aleatório no início
		srand(rand() + game_index);
	}
}