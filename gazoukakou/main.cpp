#pragma warning(disable:4996)

#include <cstdio>  
#include <iostream>  
#include <cstring>  

#define FILE_HEADER_SIZE 14                // �t�@�C���w�b�_�̃T�C�Y  
#define INFO_HEADER_SIZE 40                // ���w�b�_�̃T�C�Y  

using namespace std;

/*
* �t�@�C���w�b�_�[�\����
*/
typedef struct FileHeader
{
	uint8_t data[FILE_HEADER_SIZE];  // ���H�O�f�[�^�i�����o���̂��ߕK�v�j  
	string fileType;                 // �t�@�C���^�C�v  
	int fileSize;                    // �t�@�C���T�C�Y  
} FileHeader;

/*
* ���w�b�_�[�\����
*/
typedef struct InfoHeader
{
	uint8_t data[INFO_HEADER_SIZE];  // ���H�O�f�[�^�i�����o���̂��ߕK�v�j  
	int infoHeaderSize;              // ���w�b�_�̃T�C�Y  
	int width;                       // �摜�̕�  
	int height;                      // �摜�̍���  
	int clrPerPixel;                 // 1�s�N�Z��������̐F��  
	int dataSize;                    // �摜�f�[�^�̃T�C�Y  
} InfoHeader;

/*
* �s�N�Z���̐F���\����
*/
typedef struct Color
{
	int r;                // �Ԑ���  
	int g;                // �ΐ���  
	int b;                // ����  
} Color;

int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

/*
* �r�b�g�}�b�v�����N���X
*/
class BitMapProcessor {
	FILE* bmp;               // �r�b�g�}�b�v�̃t�@�C���|�C���^  
	uint8_t* img;            // �r�b�g�}�b�v�f�[�^�i���H�p�j  
	uint8_t* org;            // �r�b�g�}�b�v�f�[�^�i�ǂݍ��ݎ��j  
	FileHeader fHeader;      // �t�@�C���w�b�_  
	InfoHeader iHeader;      // ���w�b�_  

public:
	BitMapProcessor()
	{
		bmp = NULL;
		img = NULL;
		org = NULL;
	};

	~BitMapProcessor()
	{
		if (bmp != NULL)
			fclose(bmp);
		delete[]img;
		delete[]org;
	}
	void loadData(string filename);
	void dispBmpInfo();
	void writeData(string filename, int d);
	Color getColor(int row, int col);
	void setColor(int row, int col, int r, int g, int b);
	void restore();
	int height() { return iHeader.height; };
	int width() { return iHeader.width; };
private:
	void readFileHeader();
	void readInfoHeader();
	void readBmpData();

};

/*
* 4�r�b�g����Int�����l�ɕϊ�
*/
int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	return b1 +
		b2 * 256 +
		b3 * 256 * 256 +
		b4 * 256 * 256 * 256;
}

/*
* �r�b�g�}�b�v�f�[�^�����[�h����
*/
void BitMapProcessor::loadData(string filename)
{
	if (bmp != NULL)
		fclose(bmp);

	bmp = fopen(filename.c_str(), "rb");
	if (bmp == NULL)
		printf("�t�@�C���I�[�v���Ɏ��s���܂����B\n");

	readFileHeader();
	readInfoHeader();
	readBmpData();
}

/*
* �t�@�C���w�b�_��ǂ�
*/
void BitMapProcessor::readFileHeader()
{
	uint8_t data[FILE_HEADER_SIZE];
	size_t size = fread(data, sizeof(uint8_t), FILE_HEADER_SIZE, bmp);

	memcpy(fHeader.data, data, sizeof(data));
	fHeader.fileType = "";
	fHeader.fileType += data[0];
	fHeader.fileType += data[1];
	fHeader.fileSize = bit2Integer(data[2], data[3], data[4], data[5]);
}

/*
* ���w�b�_��ǂ�
*/
void BitMapProcessor::readInfoHeader()
{
	uint8_t data[INFO_HEADER_SIZE];
	size_t size = fread(data, sizeof(uint8_t), INFO_HEADER_SIZE, bmp);

	memcpy(iHeader.data, data, sizeof(data));
	iHeader.infoHeaderSize = bit2Integer(data[0], data[1], data[2], data[3]);
	iHeader.width = bit2Integer(data[4], data[5], data[6], data[7]);
	iHeader.height = bit2Integer(data[8], data[9], data[10], data[11]);
	iHeader.clrPerPixel = bit2Integer(data[14], data[15], 0, 0);
	iHeader.dataSize = bit2Integer(data[20], data[21], data[22], data[23]);
}

/*
* �摜�f�[�^��ǂ�
*/
void BitMapProcessor::readBmpData()
{
	if (img != NULL)
		delete[]img;

	int sz = iHeader.dataSize;
	img = new uint8_t[sz];
	size_t size = fread(img, sizeof(uint8_t), sz, bmp);
	if (size != sz)
		printf("�摜�f�[�^�ǂݍ��݂̃T�C�Y���������Ă��܂��B");

	// �o�b�N�A�b�v�p��org�ɉ摜�f�[�^���R�s�[  
	if (org != NULL)
		delete[]org;
	org = new uint8_t[sz];
	memcpy(org, img, sz);
}

/*
* �r�b�g�}�b�v���̕\���i�f�o�b�O�p�j
*/
void BitMapProcessor::dispBmpInfo()
{
	cout << "���t�@�C���w�b�_���" << endl;
	cout << "�t�@�C���^�C�v: " << fHeader.fileSize << endl;
	cout << "�t�@�C���T�C�Y: " << fHeader.fileSize << endl;

	cout << "�����w�b�_���" << endl;
	cout << "���w�b�_�T�C�Y: " << iHeader.infoHeaderSize << endl;
	cout << "�摜��: " << iHeader.width << endl;
	cout << "�摜��: " << iHeader.height << endl;
	cout << "�P�s�N�Z��������̐F��: " << iHeader.clrPerPixel << endl;
	cout << "�摜�f�[�^�̃T�C�Y: " << iHeader.dataSize << endl;
}

/*
* �r�b�g�}�b�v�f�[�^�̃t�@�C�������o��
*/
void BitMapProcessor::writeData(string filename, int d)
{
	char str[20];
	sprintf(str, "%s%d.bmp", filename.c_str(), d);

	FILE* out = fopen(str, "wb");

	if (out == NULL)
		printf("�����o����̃t�@�C�����J���܂���B\n");

	fwrite(fHeader.data, sizeof(uint8_t), FILE_HEADER_SIZE, out);
	fwrite(iHeader.data, sizeof(uint8_t), INFO_HEADER_SIZE, out);
	fwrite(img, sizeof(uint8_t), iHeader.dataSize, out);

	fclose(out);
}

/*
* �w�肳�ꂽ�s�N�Z���̐F���擾
*/
Color BitMapProcessor::getColor(int row, int col)
{
	if (row < 0 || row >= iHeader.height)
		printf("getColor(): row���͈͊O�ł��B\n");
	if (col < 0 || col >= iHeader.width)
		printf("getColor(): col���͈͊O�ł��B\n");

	int width = 3 * iHeader.width;
	while (width % 4)        // �r�b�g�}�b�v��1���4�̔{���r�b�g����Ȃ�  
		++width;

	int bPos = row * width + 3 * col;
	int gPos = bPos + 1;
	int rPos = bPos + 2;

	Color color;
	color.r = img[rPos];
	color.g = img[gPos];
	color.b = img[bPos];

	return color;
}

/*
* �w�肳�ꂽ�s�N�Z���ɐF��ݒ�
*/
void BitMapProcessor::setColor(int row, int col, int r, int g, int b)
{
	if (row < 0 || row >= iHeader.height)
		printf("getColor(): row���͈͊O�ł��B\n");
	if (col < 0 || col >= iHeader.width)
		printf("getColor(): col���͈͊O�ł��B\n");

	int width = 3 * iHeader.width;
	while (width % 4)        // �r�b�g�}�b�v��1���4�̔{���r�b�g����Ȃ�  
		++width;

	int bPos = row * width + 3 * col;
	int gPos = bPos + 1;
	int rPos = bPos + 2;

	img[rPos] = r;
	img[gPos] = g;
	img[bPos] = b;
}

/*
* �r�b�g�}�b�v�f�[�^�����H�O�ɕ�������
*/
void BitMapProcessor::restore()
{
	memcpy(img, org, iHeader.dataSize);
}


/*
* �e�X�g�p�֐��i1�j���m�N����
*/
void twoTone(BitMapProcessor * bmp)
{
	for (int i = 0; i < bmp->height(); i++)
	{

		for (int j = 0; j < bmp->width(); j++)
		{
			int ave = 0;
			ave += bmp->getColor(i, j).r;
			ave += bmp->getColor(i, j).g;
			ave += bmp->getColor(i, j).b;
			ave /= 3;

			bmp->setColor(i, j, ave, ave, ave);
		}
	}
}





/*
* �e�X�g�֐��i2�j�w��͈͂̐؂���
*/
void extractArea(BitMapProcessor * bmp, int r0, int r1, int c0, int c1)
{
	for (int i = 0; i < bmp->height(); i++)
	{
		for (int j = 0; j < bmp->width(); j++)
		{
			if (r0 <= i && i <= r1 && c0 <= j && j <= c1)
				continue;
			bmp->setColor(i, j, 255, 255, 255);
		}
	}
}

/*
* �e�X�g�֐��i3�j �F�̔��]
*/
void invert(BitMapProcessor * bmp)
{
	for (int i = 0; i < bmp->height(); i++)
		for (int j = 0; j < bmp->width(); j++)
		{
			int ave = 0;
			int r = bmp->getColor(i, j).r;
			int g = bmp->getColor(i, j).g;
			int b = bmp->getColor(i, j).b;

			bmp->setColor(i, j, 255 - r, 255 - g, 255 - b);
		}
}





void mozaic(BitMapProcessor * bmp, int level)
{
	if (level <= 0)
		level = 1;

	for (int i = 0; i < bmp->height(); i += 2 * level)
		for (int j = 0; j < bmp->width(); j += 2 * level)
		{
			int r = 0;
			int g = 0;
			int b = 0;
			int cnt = 0;

			for (int x = -level; x <= level; x++)
				for (int y = -level; y <= level; y++)
				{
					int xt = i + x;
					int yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
						continue;
					++cnt;
					r += bmp->getColor(xt, yt).r;
					g += bmp->getColor(xt, yt).g;
					b += bmp->getColor(xt, yt).b;
				}

			r /= cnt;
			g /= cnt;
			b /= cnt;

			for (int x = -level; x <= level; x++)
				for (int y = -level; y <= level; y++)
				{
					int xt = i + x;
					int yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
						continue;

					bmp->setColor(xt, yt, r, g, b);
				}
		}
}



/*
* �e�X�g�֐��i4�j���U�C�N��
*/
void mosaic(BitMapProcessor * bmp, int level)
{
	if (level <= 0)
		level = 1;

	for (int i = 0; i < bmp->height(); i += 2 * level)
		for (int j = 0; j < bmp->width(); j += 2 * level)
		{
			int r = 0;
			int g = 0;
			int b = 0;
			int cnt = 0;

			for (int x = -level; x <= level; x++)
				for (int y = -level; y <= level; y++)
				{
					int xt = i + x;
					int yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
						continue;
					++cnt;
					r += bmp->getColor(xt, yt).r;
					g += bmp->getColor(xt, yt).g;
					b += bmp->getColor(xt, yt).b;
				}

			r /= cnt;
			g /= cnt;
			b /= cnt;

			for (int x = -level; x <= level; x++)
				for (int y = -level; y <= level; y++)
				{
					int xt = i + x;
					int yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
						continue;

					bmp->setColor(xt, yt, r, g, b);
				}
		}
}

/*
* ���C������
*/
int main()
{
	BitMapProcessor bmp[100];
	char fname[50];
	int d, cnt = 0;
	while (true)
	{

		printf("�t�@�C��������͂��Ă�������");
		scanf("%s", fname);
		if (strcmp(fname, "next") == 0)
		{
			break;
		}
		else
		{
			// �r�b�g�}�b�v�f�[�^�̃��[�h  
			bmp[cnt].loadData(fname);


			// �r�b�g�}�b�v���̕\��  
			bmp[cnt].dispBmpInfo();

			cnt++;
		}
	}
	int i, t;
	char cmd[10], name[10], num[10];

	while (true)	//�����ƌJ��Ԃ�(exit�R�}���h��break)
	{
		printf("*******    �摜���H�A�v��    *******\n");
		printf("���m�N���@�� mono  \n");
		printf("�؂�o���@�� cut \n");
		printf("�F���]�� col \n");
		printf("���U�C�N�� moza \n");
		printf("�I���� exit \n");
		printf("\n");

		//�R�}���h�̓���
		scanf_s("%s", cmd, 10);
		//�啶���œ��͂����ꍇ�ł��������ɕϊ�����
		for (i = 0; i < strlen(cmd); i++)
			cmd[i]
			=
			tolower(cmd[i]);

		if (strcmp(cmd, "exit") == 0)
		{
			//exit�R�}���h�̏ꍇ
			break;
		}
		else if (strcmp(cmd, "mono") == 0)
		{
			for (d = 0; d < cnt; d++)
			{
				//mono�R�}���h�̏ꍇ
				twoTone(&bmp[d]);
				bmp[d].writeData("mono", d);
				bmp[d].restore();
			}
		}
		else if (strcmp(cmd, "cut") == 0)
		{
			for (d = 0; d < cnt; d++)
			{
				//cut�R�}���h�̏ꍇ
				//�S����ʕ\��
				extractArea(&bmp[d], 200, 300, 100, 180);
				bmp[d].writeData("cut", d);
				bmp[d].restore();
			}
		}
		else if (strcmp(cmd, "col") == 0)
		{
			for (d = 0; d < cnt; d++)
			{
				invert(&bmp[d]);
				bmp[d].writeData("col", d);
				bmp[d].restore();
			}
		}
		else if (strcmp(cmd, "moza") == 0)
		{
			for (d = 0; d < cnt; d++)
			{
				printf("���U�C�N�̑e������͂��Ă�������");
				scanf("%d", &t);
				mosaic(&bmp[d], t);
				bmp[d].writeData("moza", d);
				bmp[d].restore();
			}
		}
	}
	return 0;

}