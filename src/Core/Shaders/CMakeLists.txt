include(compile_shaders)
compile_shaders(
    TARGET Core
    SHADER_MODULE game_shaders
    
    SHADERS
	Particles/evolution_spark.vert
    Particles/evolution_spark.frag
)
