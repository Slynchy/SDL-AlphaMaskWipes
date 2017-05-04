#pragma once

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <iostream>

enum TRANS_TYPE
{
	OUT,
	IN
};

class Transition
{
	private:
		SDL_Surface* surface;
		Uint32* backupPixels;
		Uint32* pixelsPtr;
		SDL_PixelFormat* fmt;
		size_t length;
		TRANS_TYPE type;
		int counter = 0;
		int speed;
		int completionCounter = 0;

		void FromBlack()
		{
			for (size_t i = 0; i < length; i++)
			{
				// Store pointer to pixel
				Uint32* pixel = &pixelsPtr[i];
				Uint32* origPixel = &backupPixels[i];

				// Store the red value (and because grayscale, the other colors too)
				Uint32 temp = *pixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				Uint8 color = (Uint8)temp;
				temp = *origPixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				Uint32 origColor = (Uint8)temp;

				// And do the same with the alpha
				temp = *pixel & fmt->Amask;
				temp = temp >> fmt->Ashift;
				temp = temp << fmt->Aloss;
				Uint8 alpha = (Uint8)temp;

				// Calculate the alpha
				int tempAlpha = (int)alpha + (speed * 5) - (int)color;
				// Clamp it between 0 and 255
				if (tempAlpha < 0)
				{
					tempAlpha = 0;
				}
				else if (tempAlpha > 255)
				{
					tempAlpha = 255;
					completionCounter++;
				}

				// Do the same for the color
				int tempColor = (int)color - speed;
				if (tempColor < 0)
				{
					tempColor = 0;
					completionCounter++;
				}
				else if (tempColor > 255)
				{
					tempColor = 255;
				}

				// Map the RGBA values to the pixel
				*pixel = SDL_MapRGBA(fmt,
					(Uint8)tempColor,
					(Uint8)tempColor,
					(Uint8)tempColor,
					(Uint8)tempAlpha
				);
			}
		}

		void ToBlack()
		{
			for (size_t i = 0; i < length; i++)
			{
				// Store pointer to pixel
				Uint32* pixel = &pixelsPtr[i];
				Uint32* origPixel = &backupPixels[i];

				// Store the red value (and because grayscale, the other colors too)
				Uint32 temp = *pixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				Uint8 color = (Uint8)temp;
				temp = *origPixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				Uint8 origColor = (Uint8)temp;

				// And do the same with the alpha
				temp = *pixel & fmt->Amask;
				temp = temp >> fmt->Ashift;
				temp = temp << fmt->Aloss;
				Uint8 alpha = (Uint8)temp;

				if ((255 - counter) > origColor)
					continue;

				// Calculate the alpha
				int tempAlpha = alpha - speed*5;
				// Clamp it between 0 and 255
				if (tempAlpha < 0)
				{
					tempAlpha = 0;
					completionCounter+=2;
				}
				else if (tempAlpha > 255)
				{
					tempAlpha = 255;
				}

				// Map the RGBA values to the pixel
				*pixel = SDL_MapRGBA(fmt,
					(Uint8)0,
					(Uint8)0,
					(Uint8)0,
					(Uint8)tempAlpha
				);
			}
		}

		Transition()
		{

		};
	protected:
	public:
		SDL_Texture* texture;

		bool isComplete()
		{
			return (completionCounter == -1);
		}

		Transition(SDL_Renderer* _rend, const char* _name, TRANS_TYPE _type, int _speed = 3)
		{
			surface = IMG_Load(_name);
			if (surface == NULL)
			{
				printf("Failed to load texture %s\n", _name);
				return;
			}
			else
			{
				fmt = surface->format;
				if (fmt->BytesPerPixel != 4)
					printf("Improperly formatted texture; make sure to be 4bytes and 32bit!\n");
			}

			length = (surface->h * surface->pitch) / fmt->BytesPerPixel;
			pixelsPtr = (Uint32*)surface->pixels;
			backupPixels = new Uint32[length];
			memcpy(backupPixels, surface->pixels, length * 4);
			type = _type;
			speed = _speed;

			for (size_t i = 0; i < length; i++)
			{
				if (type == IN)
				{
					Uint32* pixel = &pixelsPtr[i];
					Uint32 temp = *pixel & fmt->Rmask;
					temp = temp >> fmt->Rshift;
					temp = temp << fmt->Rloss;
					Uint8 color = (Uint8)temp;
					pixelsPtr[i] = SDL_MapRGBA(fmt,
						color,
						color,
						color,
						0
					);
				}
				else if (type == OUT)
				{
					pixelsPtr[i] = SDL_MapRGBA(fmt,
						0,
						0,
						0,
						255
					);
				}
			}
			texture = SDL_CreateTextureFromSurface(_rend, surface);
		}

		~Transition()
		{
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}

		void Update(SDL_Renderer* _rend)
		{
			if (surface == NULL || completionCounter == -1) return;
			SDL_DestroyTexture(texture);
			SDL_LockSurface(surface);
			if (type == IN)
			{
				FromBlack();
			}
			else if (type == OUT)
			{
				ToBlack();
			}
			SDL_UnlockSurface(surface);
			texture = SDL_CreateTextureFromSurface(_rend, surface);
			counter += speed;
			if (completionCounter != length*2)
				completionCounter = 0;
			else
				completionCounter = -1;
		}
};