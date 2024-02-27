#include "../../dx12Engine/dx12Engine/standard.h"

#include "../../dx12Engine/dx12Engine/CBmpImage.cpp"
#include "../../dx12Engine/dx12Engine/CHeapAllocator.cpp"
#include "../../dx12Engine/dx12Engine/CHeapArray.cpp"
#include "../../dx12Engine/dx12Engine/CTerrainTile.cpp"
#include "../../dx12Engine/dx12Engine/CVertex.cpp"

errno_t err;
size_t bytesRead;

CBmpImage* bmp;

CHeapArray* verts;
CHeapArray* tiles;

CBmpImage* mask1;
CBmpImage* mask2;
CBmpImage* lightmap;

int primSize;
float vheight;
int vertCount;
int useUV;
int average;
int vertices;
    
int width;
int height;
    
int iwidth;
int iheight;

BYTE* pixelStart;

int vertexCount;

FILE* master;
FILE* finalTerrain;
FILE* collision;
FILE* mask1File;
FILE* mask2File;
FILE* lightmapFile;
FILE* verticesFile;

DWORD b;

float x;
float z;

float uinc;
float vinc;

float u;
float v;

CTerrainTile t1;
CTerrainTile t2;
CTerrainTile t3;
CTerrainTile t4;

void WriteTile(CVertex* a, CVertex* b, CVertex* c, CVertex* n1, CVertex* d, CVertex* e, CVertex* f, CVertex* n2, bool uuv, float u, float v);

CVertex CalculateNormal(CVertex* a, CVertex* b, CVertex* c);

void WriteUVCoord(float tu, float tv);

void WriteNormal(CVertex* n);

int main(int ac, char* argv[])
{
	if (strcmp(argv[1], "-h") == 0)
	{
		printf("order of arguments is:\n");
		printf("image name\n");
		printf("primitive size\n");
		printf("height\n");
		printf("vertex count\n");
		printf("use UV\n");
		printf("average vertex normals\n");
		printf("vertices\n");
		printf("oceancliffs.bmp 64 24 64 1 1 1\n");

		return 0;
	}

	bmp = new CBmpImage(argv[1]);

	primSize = (int)atoi(argv[2]);
	vheight = (float)atof(argv[3]);
	vertCount = (int)atoi(argv[4]);
	useUV = (int)atoi(argv[5]);
	average = (int)atoi(argv[6]);
	vertices = (int)atoi(argv[7]);

	width = bmp->m_bmapInfo.bmiHeader.biWidth;
	height = bmp->m_bmapInfo.bmiHeader.biHeight;

	uinc = 1.0f / width;
	vinc = 1.0f / height;

	u = 0.0f;
	v = 1.0f;

	iwidth = width / vertCount;
	iheight = height / vertCount;

	printf("Building Vertices\n");

	pixelStart = (bmp->m_pixels8 + (size_t)width * height * bmp->m_bytesPerPixel) - width;

	x = ((float)width / 2) * -primSize;
	z = ((float)height / 2) * -primSize;

	verts = new CHeapArray(sizeof(CVertex), true, false, 2, width, height);

	for (int h = 0; h < height; h++)
	{
		x = ((float)width / 2) * -primSize;

		for (int w = 0; w < width; w++)
		{
			printf("H:%03iW:%03i\r", h + 1, w + 1);

			CVertex* v = (CVertex*)verts->GetElement(2, w, h);


			v->p.x = x;

			v->p.y = (float)(bmp->m_paletteentry[*pixelStart].peRed * vheight);

			v->p.z = z;


			vertexCount++;

			x += primSize;

			pixelStart++;
		}

		z += primSize;

		pixelStart -= ((int)width * 2);
	}


	printf("\nNumber of vertices:%i\n", vertexCount);


	printf("Writing Tiles And Collision\n");

	err = fopen_s(&master, "master.txt", "wb");

	if (err)
	{
		printf("\nError opening master.txt wb\n");

		delete verts;
		delete bmp;

		return 0;
	}

	err = fopen_s(&collision, "collision.txt", "wb");

	if (err)
	{
		printf("\nError opening collision.txt wb\n");

		delete verts;
		delete bmp;

		fclose(master);

		return 0;
	}

	/*

	B
	|\
	A-C

	E-F
	 \|
	  D

	*/

	fwrite(&width, sizeof(int), 1, collision);
	fwrite(&height, sizeof(int), 1, collision);
	fwrite(&primSize, sizeof(int), 1, collision);

	for (int h = 0; h < height - 1; h++)
	{
		for (int w = 0; w < width - 1; w++)
		{
			printf("H:%03iW:%03i\r", h + 1, w + 1);

			CVertex* a = (CVertex*)verts->GetElement(2, w, h);
			CVertex* b = (CVertex*)verts->GetElement(2, w, h + 1);
			CVertex* c = (CVertex*)verts->GetElement(2, w + 1, h);

			CVertex n1 = CalculateNormal(b, a, c);

			CVertex* d = (CVertex*)verts->GetElement(2, w + 1, h);
			CVertex* e = (CVertex*)verts->GetElement(2, w, h + 1);
			CVertex* f = (CVertex*)verts->GetElement(2, w + 1, h + 1);

			CVertex n2 = CalculateNormal(d, f, e);

			WriteTile(a, b, c, &n1, d, e, f, &n2, useUV, u, v);

			u += uinc;
		}

		u = 0.0f;

		v -= vinc;
	}

	fclose(master);
	fclose(collision);

	delete bmp;
	delete verts;

	if (average)
	{
		printf("\nAveraging Vertex Normals\n");

		err = fopen_s(&master, "master.txt", "rb+");

		if (err)
		{
			printf("\nError opening master.txt rb+\n");

			return 0;
		}

		tiles = new CHeapArray(sizeof(CTerrainTile), false, false, 2, width - 1, height - 1);

		for (int h = 0; h < height - 1; h++)
		{
			for (int w = 0; w < width - 1; w++)
			{
				printf("H:%03iW:%03i\r", h + 1, w + 1);

				int w1 = ((w - 1) < 0) ? 0 : w - 1;
				int h1 = ((h - 1) < 0) ? 0 : h - 1;

				int w1h1 = tiles->GetOffset(2, w1, h1);

				fseek(master, w1h1, SEEK_SET);
				fread(&t1, sizeof(CTerrainTile), 1, master);


				int w2 = ((w - 1) < 0) ? 0 : w - 1;
				int h2 = h;

				int w2h2 = tiles->GetOffset(2, w2, h2);

				fseek(master, w2h2, SEEK_SET);
				fread(&t2, sizeof(CTerrainTile), 1, master);


				int w3 = w;
				int h3 = h;

				int w3h3 = tiles->GetOffset(2, w3, h3);

				fseek(master, w3h3, SEEK_SET);
				fread(&t3, sizeof(CTerrainTile), 1, master);


				int w4 = w;
				int h4 = ((h - 1) < 0) ? 0 : h - 1;

				int w4h4 = tiles->GetOffset(2, w4, h4);

				fseek(master, w4h4, SEEK_SET);
				fread(&t4, sizeof(CTerrainTile), 1, master);


				float sumX = t1.m_f.n.x + t2.m_c.n.x + t2.m_d.n.x + t3.m_a.n.x + t4.m_e.n.x + t4.m_b.n.x;
				float sumY = t1.m_f.n.y + t2.m_c.n.y + t2.m_d.n.y + t3.m_a.n.y + t4.m_e.n.y + t4.m_b.n.y;
				float sumZ = t1.m_f.n.z + t2.m_c.n.z + t2.m_d.n.z + t3.m_a.n.z + t4.m_e.n.z + t4.m_b.n.z;

				float aX = sumX / 6.0f;
				float aY = sumY / 6.0f;
				float aZ = sumZ / 6.0f;

				CVertex nvn = CVertex(aX, aY, aZ);

				nvn.Normalize();

				t1.m_f.n.x = t2.m_c.n.x = t2.m_d.n.x = t3.m_a.n.x = t4.m_e.n.x = t4.m_b.n.x = nvn.p.x;
				t1.m_f.n.y = t2.m_c.n.y = t2.m_d.n.y = t3.m_a.n.y = t4.m_e.n.y = t4.m_b.n.y = nvn.p.y;
				t1.m_f.n.z = t2.m_c.n.z = t2.m_d.n.z = t3.m_a.n.z = t4.m_e.n.z = t4.m_b.n.z = nvn.p.z;


				fseek(master, w1h1, SEEK_SET);
				fwrite(&t1, sizeof(CTerrainTile), 1, master);

				fseek(master, w2h2, SEEK_SET);
				fwrite(&t2, sizeof(CTerrainTile), 1, master);

				fseek(master, w3h3, SEEK_SET);
				fwrite(&t3, sizeof(CTerrainTile), 1, master);

				fseek(master, w4h4, SEEK_SET);
				fwrite(&t4, sizeof(CTerrainTile), 1, master);
			}
		}

		fclose(master);

		delete tiles;

		printf("\n");
	}



	if (vertices)
	{
		printf("Writing Vertices from tiles\n");

		err = fopen_s(&master, "master.txt", "rb");

		if (err)
		{
			printf("\nError opening master.txt rb\n");

			return 0;
		}

		err = fopen_s(&verticesFile, "vertices.txt", "wb");

		fwrite(&width, sizeof(int), 1, verticesFile);
		fwrite(&height, sizeof(int), 1, verticesFile);
		fwrite(&primSize, sizeof(int), 1, verticesFile);

		tiles = new CHeapArray(sizeof(CTerrainTile), false, false, 2, width - 1, height - 1);

		int c = 0;

		for (int h = 0; h < height - 1; h++)
		{
			for (int w = 0; w < width - 1; w++)
			{
				printf("H:%03iW:%03i\r", h + 1, w + 1);

				int wh = tiles->GetOffset(2, w, h);

				fseek(master, wh, SEEK_SET);
				fread(&t1, sizeof(CTerrainTile), 1, master);

				fwrite(&t1.m_a, sizeof(CVertexNT), 1, verticesFile);

				c++;
			}

			fwrite(&t1.m_c, sizeof(CVertexNT), 1, verticesFile);

			c++;
		}

		for (int w = 0; w < width - 1; w++)
		{
			printf("H:%03iW:%03i\r", height + 1, width);

			// height - 2 for last tile
			int wh = tiles->GetOffset(2, w, height - 2);

			fseek(master, wh, SEEK_SET);
			fread(&t1, sizeof(CTerrainTile), 1, master);

			fwrite(&t1.m_b, sizeof(CVertexNT), 1, verticesFile);

			c++;
		}

		fwrite(&t1.m_f, sizeof(CVertexNT), 1, verticesFile);

		c++;

		printf("\ncount:%i", c);


		if (master)
		{
			fclose(master);
		}

		if (verticesFile)
		{
			fclose(verticesFile);
		}

		delete tiles;

		printf("\n");
	}



	printf("Writing Final Terrain\n");

	fopen_s(&master, "master.txt", "rb");
	fopen_s(&finalTerrain, "terrain.txt", "wb");

	if (finalTerrain)
	{
		fwrite(&width, sizeof(int), 1, finalTerrain);
		fwrite(&height, sizeof(int), 1, finalTerrain);
		fwrite(&primSize, sizeof(int), 1, finalTerrain);
	}

	
	if (master)
	{
		fread_s(&b, sizeof(DWORD), sizeof(DWORD), 1, master);

		while (!feof(master))
		{
			if (finalTerrain)
			{
				fwrite(&b, sizeof(DWORD), 1, finalTerrain);
			}

			fread_s(&b, sizeof(DWORD), sizeof(DWORD), 1, master);
		}
	}

	
	if (finalTerrain)
	{
		fclose(finalTerrain);
	}

	if (master)
	{
		fclose(master);
	}



	if (1)
	{
		printf("Writing Blend Masks\n");

		err = fopen_s(&master, "master.txt", "rb");

		if (err)
		{
			return -1;
		}

		err = fopen_s(&mask1File, "mask1.bmp", "wb");

		if (err)
		{
			return -1;
		}

		mask1 = new CBmpImage();
		mask1->WriteBitmapHeader(width, height, mask1File);
		mask1->WriteGreyscalePalette(mask1File);

		err = fopen_s(&mask2File, "mask2.bmp", "wb");

		if (err)
		{
			return -1;
		}

		mask2 = new CBmpImage();
		mask2->WriteBitmapHeader(width, height, mask2File);
		mask2->WriteGreyscalePalette(mask2File);

		err = fopen_s(&lightmapFile, "lightmap.bmp", "wb");

		if (err)
		{
			return -1;
		}

		lightmap = new CBmpImage();
		lightmap->WriteBitmapHeader(width, height, lightmapFile);
		lightmap->WriteGreyscalePalette(lightmapFile);

		tiles = new CHeapArray(sizeof(CTerrainTile), false, true, 2, width - 1, height - 1);

		BYTE p = 0;

		for (int h = 0; h < height - 1; h++)
		{
			for (int w = 0; w < width - 1; w++)
			{
				printf("H:%03iW:%03i\r", h + 1, w + 1);

				int w1h1 = tiles->GetOffset(2, w, h);

				fseek(master, w1h1, SEEK_SET);
				fread(&t1, sizeof(CTerrainTile), 1, master);

				CVertex n = CVertex(t1.m_a.n.x, t1.m_a.n.y, t1.m_a.n.z);

				float NdL = n.Dot(CVertex(0.0f, -1.0f, 0.0f));

				float NdLl = n.Dot(CVertex(0.0f, -1.0f, 0.0f));

				p = (BYTE)(255 * fabs(NdLl));
				fwrite(&p, sizeof(BYTE), 1, lightmapFile);

				p = 0;
				if (NdL > -0.65)
				{
					p = 255;
				}
				fwrite(&p, sizeof(BYTE), 1, mask1File);

				p = 0;
				if ((NdL < -0.65) && (NdL > -0.80))
				{
					p = 255;
				}
				fwrite(&p, sizeof(BYTE), 1, mask2File);
			}
			p = 0;

			fwrite(&p, sizeof(BYTE), 1, mask1File);
			fwrite(&p, sizeof(BYTE), 1, mask2File);
			fwrite(&p, sizeof(BYTE), 1, lightmapFile);
		}

		p = 0;

		for (int w = 0; w < width; w++)
		{
			fwrite(&p, sizeof(BYTE), 1, mask1File);
			fwrite(&p, sizeof(BYTE), 1, mask2File);
			fwrite(&p, sizeof(BYTE), 1, lightmapFile);
		}

		if (master)
		{
			fclose(master);
		}

		if (mask1File)
		{
			fclose(mask1File);
		}

		if (mask2File)
		{
			fclose(mask2File);
		}

		if (lightmapFile)
		{
			fclose(lightmapFile);
		}

		printf("\n");
	}

	printf("Complete\n");

	//system("pause");

	return 0;
}

CVertex CalculateNormal(CVertex* a, CVertex* b, CVertex* c)
{
	CVertex edge1 = *c - *b;
	CVertex edge2 = *c - *a;

	edge1.Normalize();

	edge2.Normalize();

	CVertex N1 = edge1.Cross(edge2);

	N1.Normalize();

	return N1;
}

void WriteTile(CVertex* a, CVertex* b, CVertex* c, CVertex* n1, CVertex* d, CVertex* e, CVertex* f, CVertex* n2, bool uuv, float u, float v)
{
	fwrite(a, sizeof(CVertex), 1, master);
	WriteNormal(n1);

	if (uuv)
	{
		WriteUVCoord(u, v);
	}
	else
	{
		WriteUVCoord(0.0f, 1.0f);
	}

	fwrite(b, sizeof(CVertex), 1, master);
	WriteNormal(n1);

	if (uuv)
	{
		WriteUVCoord(u, v - vinc);
	}
	else
	{
		WriteUVCoord(0.0f, 0.0f);
	}


	fwrite(c, sizeof(CVertex), 1, master);
	WriteNormal(n1);

	if (uuv)
	{
		WriteUVCoord(u + uinc, v);
	}
	else
	{
		WriteUVCoord(1.0f, 1.0f);
	}



	fwrite(d, sizeof(CVertex), 1, master);
	WriteNormal(n2);

	if (uuv)
	{
		WriteUVCoord(u + uinc, v);
	}
	else
	{
		WriteUVCoord(1.0f, 1.0f);
	}


	fwrite(e, sizeof(CVertex), 1, master);
	WriteNormal(n2);

	if (uuv)
	{
		WriteUVCoord(u, v - vinc);
	}
	else
	{
		WriteUVCoord(0.0f, 0.0f);
	}


	fwrite(f, sizeof(CVertex), 1, master);
	WriteNormal(n2);

	if (uuv)
	{
		WriteUVCoord(u + uinc, v - vinc);
	}
	else
	{
		WriteUVCoord(1.0f, 0.0f);
	}


	fwrite(a, sizeof(CVertex), 1, collision);
	fwrite(b, sizeof(CVertex), 1, collision);
	fwrite(c, sizeof(CVertex), 1, collision);

	fwrite(n1, sizeof(CVertex), 1, collision);

	fwrite(d, sizeof(CVertex), 1, collision);
	fwrite(e, sizeof(CVertex), 1, collision);
	fwrite(f, sizeof(CVertex), 1, collision);

	fwrite(n2, sizeof(CVertex), 1, collision);
}

void WriteNormal(CVertex* n)
{
	fwrite(n, sizeof(CVertex), 1, master);
}

void WriteUVCoord(float u, float v)
{
	fwrite(&u, sizeof(float), 1, master);
	fwrite(&v, sizeof(float), 1, master);
}
