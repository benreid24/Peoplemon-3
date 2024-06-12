include(compile_shaders)
compile_shaders(
    TARGET Core
    SHADER_MODULE game_shaders
    
    SHADERS
    Battle/trainerIntro.frag
    Battle/wildIntro.frag
	Particles/evolution_spark.vert
    Particles/evolution_spark.frag
    Particles/raindrop.vert
    Particles/snowflake.vert
    Particles/fog.vert
    Particles/sand.vert
    Particles/sandSwirl.vert
)
