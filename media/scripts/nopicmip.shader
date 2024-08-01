bulletExplosionnopicmip
{
	nopicmip
	cull disable
	/*
	{
		animmap 4 models/weaphits/bullet1.tga models/weaphits/bullet2.tga models/weaphits/bullet3.tga gfx/colors/black.tga
		rgbGen wave inversesawtooth 0 1 0 5
		blendfunc add
	}
	{
		animmap 4 models/weaphits/bullet2.tga models/weaphits/bullet3.tga gfx/colors/black.tga gfx/colors/black.tga
		rgbGen wave sawtooth 0 1 0 5
		blendfunc add
	}
	*/
	{
		animmap 4 models/weaphits/rlboom/rlboom_1.tga  models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_7.tga
		rgbGen wave inversesawtooth 0 1 0 8
		blendfunc add
	}
	{
		animmap 4 models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_8.tga
		rgbGen wave sawtooth 0 1 0 8
		blendfunc add
	}
}

lightningBoltNewnopicmip
{
	nopicmip
	cull none
	{
		map gfx/misc/lightning3new.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.5 0 7.1
		tcmod scale  2 1
		tcMod scroll -5 0
	}
	{
		map gfx/misc/lightning3new.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.8 0 8.1
		tcmod scale  -1.3 -1
		tcMod scroll -7.2 0
	}
}

lightningBoltNewnopicmipB
{
	nopicmip
	cull none
	{
		map gfx/misc/lightning3newb.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.5 0 7.1
		tcmod scale  2 1
		tcMod scroll -5 0
	}
	{
		map gfx/misc/lightning3newb.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.8 0 8.1
		tcmod scale  -1.3 -1
		tcMod scroll -7.2 0
	}
}

lightningBoltNewnopicmipR
{
	nopicmip
	cull none
	{
		map gfx/misc/lightning3newr.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.5 0 7.1
		tcmod scale  2 1
		tcMod scroll -5 0
	}
	{
		map gfx/misc/lightning3newr.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave sin 1 0.8 0 8.1
		tcmod scale  -1.3 -1
		tcMod scroll -7.2 0
	}
}

sprites/plasma1nopicmip
{
	nopicmip
	cull disable
	{
		clampmap sprites/plasmaa.tga
		blendfunc GL_ONE GL_ONE
            tcMod rotate -931
	}
}

plasmaExplosionnopicmip
{
	nopicmip
	cull disable
	{
		clampmap models/weaphits/plasmaboom.tga
		blendfunc add
            tcMod stretch triangle .6 0.1 0 8
            tcmod rotate 999
            rgbGen wave inversesawtooth 0 1 0 1.5
	}
}

railDiscnopicmip
{
	nopicmip
	sort nearest
	cull none
      deformVertexes wave 100 sin 0 .5 0 2.4
	{
		clampmap gfx/misc/raildisc_mono2.tga 
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
            tcMod rotate -30
	}
}

railCorenopicmip
{
	nopicmip
	sort nearest
	cull none
	{
		map gfx/misc/railcorethin_mono.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
		tcMod scroll -1 0
	}
}

railExplosionnopicmip
{
	nopicmip
	cull disable
      {
		animmap 5 models/weaphits/ring02_1.tga  models/weaphits/ring02_2.tga  models/weaphits/ring02_3.tga models/weaphits/ring02_4.tga gfx/colors/black.tga
		alphaGen wave inversesawtooth 0 1 0 5
		blendfunc blend
	}
	{
		animmap 5 models/weaphits/ring02_2.tga  models/weaphits/ring02_3.tga models/weaphits/ring02_4.tga gfx/colors/black.tga gfx/colors/black.tga
		alphaGen wave sawtooth 0 1 0 5
		blendfunc blend
	}
}

rocketExplosionnopicmip
{
	nopicmip
	cull disable
	{
		animmap 8 models/weaphits/rlboom/rlboom_1.tga  models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga
		rgbGen wave inversesawtooth 0 1 0 8
		blendfunc add
	}
	{
		animmap 8 models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga gfx/colors/black.tga
		rgbGen wave sawtooth 0 1 0 8
		blendfunc add
	}
}

grenadeExplosionnopicmip
{
	nopicmip
	cull disable
	{
		animmap 5 models/weaphits/glboom/glboom_1.tga  models/weaphits/glboom/glboom_2.tga models/weaphits/glboom/glboom_3.tga
		rgbGen wave inversesawtooth 0 1 0 5
		blendfunc add
	}
	{
		animmap 5 models/weaphits/glboom/glboom_2.tga  models/weaphits/glboom/glboom_3.tga gfx/colors/black.tga
		rgbGen wave sawtooth 0 1 0 5
		blendfunc add
	}
}

bfgExplosionnopicmip
{
	nopicmip
	cull disable
	{
		animmap 5 models/weaphits/bfgboom/bfgboom_1.tga  models/weaphits/bfgboom/bfgboom_2.tga models/weaphits/bfgboom/bfgboom_3.tga
		rgbGen wave inversesawtooth 0 1 0 5
		blendfunc add
	}
	{
		animmap 5 models/weaphits/bfgboom/bfgboom_2.tga models/weaphits/bfgboom/bfgboom_3.tga gfx/colors/black.tga
		rgbGen wave sawtooth 0 1 0 5
		blendfunc add
	}
}

bloodExplosionnopicmip
{
	nopicmip
	cull disable
	{
		animmap 5 models/weaphits/blood201.tga models/weaphits/blood202.tga models/weaphits/blood203.tga models/weaphits/blood204.tga models/weaphits/blood205.tga
		blendfunc blend
	}
}

smokePuffnopicmip
{
	nopicmip
	cull disable
	entityMergable
	{
		map gfx/misc/smokepuff3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}

smokePuffRagePronopicmip
{
	nopicmip
	cull none
	entityMergable
	{
		map gfx/misc/smokepuffragepro.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

shotgunSmokePuffnopicmip
{
	nopicmip
	cull disable
	{
		map gfx/misc/smokepuff2b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen entity		
		rgbGen entity
	}
}

nailtrailnopicmip
{
	nopicmip
	sort nearest
	cull none
	{
		clampmap models/weaphits/nailtrail.tga 
		blendFunc Add
		rgbGen vertex
            tcMod rotate -30
	}

}

waterBubblenopicmip
{
	nopicmip
	sort	underwater
	cull none
	entityMergable
	{
		map sprites/bubble.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen	vertex
		alphaGen	vertex
	}
}

powerups/battleSuitnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 1 0 0 0
	{
		map textures/effects/envmapgold2.tga
		tcGen environment
		tcMod turb 0 0.15 0 0.3
		tcmod rotate 333
		tcmod scroll .3 .3
		blendfunc GL_ONE GL_ONE
	}
}

powerups/battleWeaponnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/envmapgold2.tga
		tcGen environment
		tcMod turb 0 0.15 0 0.3
		tcmod rotate 333
		tcmod scroll .3 .3
		blendfunc GL_ONE GL_ONE
	}
}

powerups/invisibilitynopicmip
{
	nopicmip
	{
		map textures/effects/invismap.tga
		blendfunc GL_ONE GL_ONE
		tcMod turb 0 0.15 0 0.25
		tcGen environment
	}
}

powerups/quadnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 30
		tcmod scroll 1 .1
	}
}

powerups/quadWeaponnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 30
		tcmod scroll 1 .1
	}
}

powerups/regennopicmip
{
	nopicmip
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/regenmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 30
		tcmod scroll 1 .1
	}
}

hasteSmokePuffnopicmip
{
	nopicmip
	cull none
	entityMergable
	{
		map gfx/misc/smokepuff3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen	vertex
		alphaGen	vertex
	}
}

sprites/balloon3nopicmip
{
	nopicmip
	{
		map sprites/balloon4.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

gfx/damage/bullet_mrknopicmip
{
	nopicmip
	polygonOffset
	{
		map gfx/damage/bullet_mrk.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}

gfx/damage/burn_med_mrknopicmip
{
	nopicmip
	polygonOffset
	{
		map gfx/damage/burn_med_mrk.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}

gfx/damage/hole_lg_mrknopicmip
{
	nopicmip
	polygonOffset
	{
		map gfx/damage/hole_lg_mrk.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}

gfx/damage/plasma_mrknopicmip
{
	nopicmip
	polygonOffset
	{
		map gfx/damage/plasma_mrk.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		alphaGen vertex
	}
}

powerups/battleSuitnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 1 0 0 0
	{
		map textures/effects/envmapgold2.tga
		tcGen environment
		tcMod turb 0 0.15 0 0.3
            tcmod rotate 333
            tcmod scroll .3 .3
		blendfunc GL_ONE GL_ONE
	}
}

powerups/battleWeaponnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/envmapgold2.tga
		tcGen environment
		tcMod turb 0 0.15 0 0.3
            tcmod rotate 333
            tcmod scroll .3 .3
		blendfunc GL_ONE GL_ONE
	}
}

powerups/invisibilitynopicmip
{
	nopicmip
	{
		map textures/effects/invismap.tga
		blendfunc GL_ONE GL_ONE
		tcMod turb 0 0.15 0 0.25
		tcGen environment
	}
}

powerups/quadnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
            tcmod rotate 30
            tcmod scroll 1 .1
	}
}

powerups/quadWeaponnopicmip
{
	nopicmip
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

powerups/regennopicmip
{
	nopicmip
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/regenmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
            tcmod rotate 30
            tcmod scroll 1 .1
	}
}

models/weapons/nailgun/nail
{
	nopicmip     
	cull disable
        {
		map models/weapons/nailgun/nail.tga
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
}

models/weapons/nailgun/nail2
{
	nopicmip
	cull disable
        {
            map models/weapons/nailgun/nail2.tga
            alphaFunc GE128
		depthWrite
            rgbGen identity
        }
}