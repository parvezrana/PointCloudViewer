#include "drawer.h"

#define FPS 30
#define WW 600
#define WH 600

// [[Rcpp::export]]
void plotxyz(NumericVector x, NumericVector y, NumericVector z, IntegerVector r, IntegerVector g, IntegerVector b, IntegerVector id, float size = 2)
{
  bool run = true;

  SDL_Event event;
  const Uint32 time_per_frame = 1000/FPS;
  unsigned int width = WW;
  unsigned int height = WH;

  Uint32 last_time,current_time,elapsed_time;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_WM_SetCaption("Point Cloud Viewer", NULL);
  SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_RESIZABLE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70,(double)width/height,0.1,100000);

  glEnable(GL_DEPTH_TEST);

  Drawer* drawer = new Drawer(x,y,z, r, g, b, id);
  drawer->camera->setRotateSensivity(0.3);
  drawer->camera->setZoomSensivity(10);
  drawer->camera->setPanSensivity(1);
  drawer->setPointSize(size);

  last_time = SDL_GetTicks();

  while (run)
  {
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
      case SDL_QUIT:
        run = false;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
          run = false;
          break;
        default :
          drawer->camera->OnKeyboard(event.key);
        break;
        }
        break;
      case SDL_MOUSEMOTION:
        drawer->camera->OnMouseMotion(event.motion);
        break;
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        drawer->camera->OnMouseButton(event.button);
        break;
      case SDL_VIDEORESIZE:
        width = event.resize.w;
        height = event.resize.h;
        SDL_SetVideoMode(width, height, 32, SDL_OPENGL| SDL_RESIZABLE);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70,(double)width/height,0.1,10000);
        drawer->camera->changed = true;
        break;
      }
    }

    current_time = SDL_GetTicks();
    elapsed_time = current_time - last_time;

    if (elapsed_time > time_per_frame)
    {
      drawer->draw();
      last_time = current_time;
    }
    else
    {
      SDL_Delay(time_per_frame - elapsed_time);
    }
  }

  delete drawer;
  SDL_Quit();
  return;
}

