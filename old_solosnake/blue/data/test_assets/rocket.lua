mesh { 
    name = "rocket",
    modelname = "rocket",
    contents = "Triangles",
    vertices = 
    { 
        -- 25 points.
		-- Faces along -Z axis.
		-- Tail, outer
		 0.5, 0.5,1.0,				-- 0
		 0.5,-0.5,1.0,
		-0.5, 0.5,1.0,
		-0.5,-0.5,1.0,
		-- Inner tailpoint
		0.0,0.0,0.75,				-- 4
		-- Body tailpoint
		0.0,0.0,0.5,				-- 5
		-- Body, back
		 0.5, 0.5,0.5,				-- 6
		 0.5,-0.5,0.5,				-- 7
		-0.5, 0.5,0.5,				-- 8
		-0.5,-0.5,0.5,				-- 9
		-- Body, front
		 0.5, 0.5,-0.5,				-- 10
		 0.5,-0.5,-0.5,				-- 11
		-0.5, 0.5,-0.5,
		-0.5,-0.5,-0.5,				-- 13
		-- Nosecone, point
		0.0,0.0,-1.0,				-- 14
		-- Body, back, for backwall
		 0.5, 0.5,0.5,				-- 15
		 0.5,-0.5,0.5,				-- 16
		-0.5, 0.5,0.5,				-- 17
		-0.5,-0.5,0.5,				-- 18
		-- Body, front, for nosecone
		 0.5, 0.5,-0.5,				-- 19
		 0.5,-0.5,-0.5,				-- 20
		-0.5, 0.5,-0.5,				-- 21
		-0.5,-0.5,-0.5,				-- 22
		-- Tail, Inner
		 0.5, 0.5, 1.0,				-- 23
		 0.5,-0.5, 1.0,				-- 24
		-0.5, 0.5, 1.0,				-- 25
		-0.5,-0.5, 1.0,				-- 26
		-- Body, back, repeated
		 -0.5, 0.5, 0.5,			-- 27
		-- Body, front, repeated
		-0.5, 0.5,-0.5,				-- 28
		
    },
    indices  = 
    { 
		-- Tail, inner
		23,4,24,
		24,4,26,
		26,4,25,
		25,4,23,
		-- Tail, outer
		1,5,0,
		3,5,1,
		2,5,3,
		0,5,2,
		-- Nose cone
		20,14,19,
		22,14,20,
		21,14,22,
		19,14,21,
		-- Back wall
		17,18,15,
		15,18,16,		
		-- Side walls
		-- Top
		28,27,10,
		10,27,6,
		-- Bottom
		13,11,9,
		9,11,7,
		-- +X
		10,6,11,
		11,6,7,
		-- -X
		12,13,8,
		 8,13,9,		
    },
}

textureinfo {
    name = "rocket",
    modelname = "rocket",
    texture_coords = 
    {
		-- Tail, Outer
		 0.0, 0.5,					-- 0
		 0.0, 1.0,
		 0.5, 0.5,
		 0.5, 1.0,
		-- Inner tailpoint
		0.125,0.375,				-- 4
		-- Body tailpoint
		0.25,0.75,					-- 5
		-- Body, back
		 0.25, 0.25,				-- 6
		 0.5,0.25,					-- 7
		 1.0, 0.25,					-- 8
		 0.75, 0.25,				-- 9
		-- Body, front
		 0.25, 0.0,					-- 10
		 0.5, 0.0,					-- 11
		 1.0, 0.0,     				-- 12
		 0.75, 0.0,					-- 13
		-- Nosecone, point
		 0.75, 0.75,				-- 14
		-- Body, back, for backwall
		 1.0, 0.25,					-- 15
		 1.0, 0.5,					-- 16
		 0.75, 0.25,				-- 17
		 0.75, 0.5,					-- 18
		-- Body, front, for nosecone
		 0.5,0.5, 					-- 19
		 0.5,1.0, 					-- 20
		 1.0,0.5, 					-- 21
		 1.0,1.0, 					-- 22
		 -- Tail, Inner
		 0.25, 0.25,				-- 23
		 0.25, 0.5,					-- 24
		 0.0,  0.25,				-- 25
		 0.0, 0.5,					-- 26
		-- Body, back, repeated
		 0.0, 0.25,					-- 27
		-- Body, front, repeated
		 0.0, 0.0,					-- 28
    },
    diffuse_and_team = "rocket.diffuseteam.bmp",
    normals_with_occlusion_and_specular = "rocket.normalsspec.bmp",
    emissive = "rocket.emissive.bmp",
}


mesh { 
    name = "floor",
    modelname = "floor",
    contents = "Triangles",
    vertices = 
    { 	
		-10,0,-10,
		 10,0,-10,
		-10,0, 10,
		 10,0, 10
    },
    indices  = 
    { 
		1,0,2,2,3,1
    },
}


textureinfo {
    name = "floor",
    modelname = "floor",
    texture_coords = 
    {
		 0.0, 0.0,
		 1.0, 0.0,
		 0.0, 1.0,
		 1.0, 1.0,
    },
    diffuse_and_team = "floor.diffuseteam.bmp",
    normals_with_occlusion_and_specular = "floor.normalsspec.bmp",
    emissive = "floor.emissive.bmp",
}
