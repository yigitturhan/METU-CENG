all:
	g++ main.cpp -o main \
        `pkg-config --cflags --libs freetype2` \
         -g -lglfw -lpthread -lX11 -ldl -lXrandr -lGLEW -lGL -lGLU -lglut -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I.
