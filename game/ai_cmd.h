// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_cmd.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 * $Author: ben $ 
 * $Revision: 1.1.1.1 $
 * $Modtime: 11/10/99 3:30p $
 * $Date: 2001/12/11 07:13:28 $
 *
 *****************************************************************************/

extern int notleader[MAX_CLIENTS];

int BotMatchMessage(bot_state_t *bs, char *message);
void BotPrintTeamGoal(bot_state_t *bs);

