lightningBoltNewB
{
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

lightningBoltNewR
{
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

sprites/rplasma
{
	cull disable
	{
		clampmap sprites/rplasma.tga
		blendfunc GL_ONE GL_ONE
            tcMod rotate -931
	}
}

sprites/bplasma
{
	cull disable
	{
		clampmap sprites/bplasma.tga
		blendfunc GL_ONE GL_ONE
            tcMod rotate 931
	}
}

sprites/rplasmanopicmip
{
	nopicmip
	cull disable
	{
		clampmap sprites/rplasma.tga
		blendfunc GL_ONE GL_ONE
            tcMod rotate -931
	}
}

sprites/bplasmanopicmip
{
	nopicmip
	cull disable
	{
		clampmap sprites/bplasma.tga
		blendfunc GL_ONE GL_ONE
            tcMod rotate 931
	}
}

plasmaExplosionR
{
	cull disable
	{
		clampmap models/weaphits/plasmaboom_red.tga
		blendfunc add
            tcMod stretch triangle .6 0.1 0 8
            tcmod rotate 999
            rgbGen wave inversesawtooth 0 1 0 1.5
	}
}

plasmaExplosionB
{
	cull disable
	{
		clampmap models/weaphits/plasmaboom_blue.tga
		blendfunc add
            tcMod stretch triangle .6 0.1 0 8
            tcmod rotate 999
            rgbGen wave inversesawtooth 0 1 0 1.5
	}
}

plasmaExplosionRnopicmip
{
	nopicmip
	cull disable
	{
		clampmap models/weaphits/plasmaboom_red.tga
		blendfunc add
            tcMod stretch triangle .6 0.1 0 8
            tcmod rotate 999
            rgbGen wave inversesawtooth 0 1 0 1.5
	}
}

plasmaExplosionBnopicmip
{
	nopicmip
	cull disable
	{
		clampmap models/weaphits/plasmaboom_blue.tga
		blendfunc add
            tcMod stretch triangle .6 0.1 0 8
            tcmod rotate 999
            rgbGen wave inversesawtooth 0 1 0 1.5
	}
}