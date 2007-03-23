#include <stdlib.h>
#include <inttypes.h>
#include "logic.h"
#include "piece.h"
#include "playfield.h"
#include "view.h"
#include "input.h"

void tetris ()
{
	int8_t *pnWidth = (int8_t *)malloc(sizeof(int8_t));
	int8_t *pnHeight = (int8_t *)malloc(sizeof(int8_t));
	tetris_view_getDimensions(pnWidth, pnHeight);
	
    tetris_playfield_t *pPl = tetris_playfield_construct(*pnWidth, *pnHeight);
    tetris_view_t *pView = tetris_view_construct(pPl);
	tetris_input_t *pIn = tetris_input_construct();
    
    tetris_input_command_t cmd;
    tetris_piece_t *pPiece = NULL;
    tetris_piece_t *pOldPiece = NULL;
    tetris_piece_t *pNextPiece = pPiece =
    	tetris_piece_construct(rand() % 7, TETRIS_PC_ANGLE_0);
    
    while (tetris_playfield_getStatus(pPl) != TETRIS_PFS_GAMEOVER)
    {
    	switch (tetris_playfield_getStatus(pPl))
    	{
			case TETRIS_PFS_READY:
				pPiece = pNextPiece;
				pNextPiece =
					tetris_piece_construct(rand() % 7, TETRIS_PC_ANGLE_0);
				tetris_view_updateNextPiece(pView, pNextPiece);
				tetris_playfield_insertPiece(pPl, pPiece, &pOldPiece);
			    if (pOldPiece != NULL)
			    {
			        tetris_piece_destruct(pOldPiece);
			        pOldPiece = NULL;
			    }
				break;
				tetris_playfield_advancePiece(pPl);
				
			case TETRIS_PFS_HOVERING:
				cmd = tetris_input_getCommand(pIn);
				switch (cmd)
				{
					case TETRIS_INCMD_DOWN:
						tetris_playfield_advancePiece(pPl);
						break;
					case TETRIS_INCMD_LEFT:
						tetris_playfield_movePiece(pPl, TETRIS_PFD_LEFT);
						break;
					case TETRIS_INCMD_RIGHT:
						tetris_playfield_movePiece(pPl, TETRIS_PFD_RIGHT);
						break;
					case TETRIS_INCMD_ROTATE_CLOCKWISE:
						tetris_playfield_rotatePiece(pPl, TETRIS_PC_ROT_CLOCKWISE);
						break;
					case TETRIS_INCMD_ROTATE_COUNTERCLOCKWISE:
						tetris_playfield_rotatePiece(pPl, TETRIS_PC_ROT_COUNTERCLOCKWISE);
						break;
					case TETRIS_INCMD_UP:
						/* emulate immediate drop */
						while (tetris_playfield_getStatus(pPl) == TETRIS_PFS_HOVERING)
						{
							tetris_playfield_advancePiece(pPl);
						}
						break;
					case TETRIS_INCMD_NONE:
						/* nothing to do */
						break;
				}
				break;
				
			case TETRIS_PFS_DOCKED:
				tetris_playfield_removeCompleteLines(pPl);
				break;
    	}
    	
    	tetris_view_updatePlayfield(pView);
    }

	free(pnWidth);
	free(pnHeight);
	tetris_input_destruct(pIn);
    tetris_view_destruct(pView);
    tetris_playfield_destruct(pPl);
    tetris_piece_destruct(pPiece);
    tetris_piece_destruct(pNextPiece);
}

