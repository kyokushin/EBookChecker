#include <opencv2/opencv.hpp>
#include <vector>

#include "ImageScrap.h"

using namespace std;

const int Range::VERTICAL(0);
const int Range::HORIZONTAL(1);

//������������ׂ�
//�����̂Ȃ��͈͂�vector�ŕԂ�
void findSameValueHorizontal(const cv::Mat& src, std::vector<Range>& ranges)
{
	//�ϕ��摜���~����
	CV_Assert(src.type() == CV_32SC1);

	//�l�������Ă��邩������Ȃ��̂ŋ�ɂ���
	ranges.clear();

	//�ϕ��摜��int�Ȃ̂�int�^�̃|�C���^���擾�B���[�Ȃ̂ňʒu��src.rows - 1
	const int* srcLine = src.ptr<int>(src.rows - 1);

	Range range;
	range.dir = Range::HORIZONTAL;
	for (int i = 1; i < src.cols; i++){
		//���ׂƓ����l
		bool sameValue = srcLine[i] == srcLine[i - 1];
		//���ׂƓ����l ���� �͈͂�start�������l�i-1�j�̂Ƃ�
		if (sameValue && range.start < 0){
			//�����̂Ȃ��͈͂̎n�܂�
			range.start = i - 1;
		}
		//���ׂƈႤ�l ���� �͈͂�start������ς�
		else if (!sameValue && range.start >= 0){

			//�����̂Ȃ��͈͂̏I���
			range.end = i - 1;
			//���ʂƂ��ĕۑ�
			ranges.push_back(range);
			//�����̂Ȃ��͈͂������l�ɖ߂�
			range.start = -1;
			range.end = -1;
		}
	}
	//�Ō�͈̔͂��摜�̉E�[�܂ł���ꍇ��for���𔲂��Ă��猋�ʂ�ۑ�����
	//�����̂Ȃ��͈͂�start�͑���ς� ���� �͈͂�end�͏����l�̂Ƃ�
	if (range.start >= 0 && range.end < 0){
		range.end = src.cols - 1;
		ranges.push_back(range);
	}
}

//�c����������ׂ�
//�����̂Ȃ��͈͂�vector�ŕԂ�
void findSameValueVertical(const cv::Mat& src, std::vector<Range>& ranges)
{
	//�ϕ��摜���~����
	CV_Assert(src.type() == CV_32SC1);

	//�l�������Ă��邩������Ȃ��̂ŋ�ɂ���
	ranges.clear();


	Range range;
	range.dir = Range::VERTICAL;

	const int endPos = src.cols - 1;
	int src0 = src.ptr<int>(0)[endPos];
	int src1;

	for (int i = 1; i < src.rows; i++){
		src1 = src.ptr<int>(i)[endPos];

		//��ׂƓ����l
		bool sameValue = src0 == src1;
		//���ׂƓ����l ���� �͈͂�start�������l�i-1�j�̂Ƃ�
		if (sameValue && range.start < 0){
			//�����̂Ȃ��͈͂̎n�܂�
			range.start = i - 1;
		}
		//���ׂƈႤ�l ���� �͈͂�start������ς�
		else if (!sameValue && range.start >= 0){

			//�����̂Ȃ��͈͂̏I���
			range.end = i - 1;
			//���ʂƂ��ĕۑ�
			ranges.push_back(range);
			//�����̂Ȃ��͈͂������l�ɖ߂�
			range.start = -1;
			range.end = -1;
		}

		src0 = src1;
	}
	//�Ō�͈̔͂��摜�̉E�[�܂ł���ꍇ��for���𔲂��Ă��猋�ʂ�ۑ�����
	//�����̂Ȃ��͈͂�start�͑���ς� ���� �͈͂�end�͏����l�̂Ƃ�
	if (range.start >= 0 && range.end < 0){
		range.end = src.rows - 1;
		ranges.push_back(range);
	}
}

void drawRange(const cv::Mat& src, const vector<Range>& ranges, cv::Mat& dst,
	const cv::Scalar& colorVertical, const cv::Scalar& colorHorizontal){

	if (&src != &dst && dst.size() != src.size()){
		src.copyTo(dst);
	}

	for (size_t i = 0; i < ranges.size(); i++){
		const Range& r = ranges[i];
		if (r.dir == Range::VERTICAL){
			//�����̂Ȃ��͈͂�3�`�����l���̌��摜����؂�o��
			cv::Rect rect(0, r.start, dst.cols, r.end - r.start);
			cv::Mat roi(dst, rect);
			//�؂�o�����摜��1�F�œh��Ԃ�
			roi = colorVertical;
		}
		else if (r.dir == Range::HORIZONTAL){
			//�����̂Ȃ��͈͂�3�`�����l���̌��摜����؂�o��
			cv::Rect rect(r.start, 0, r.end - r.start, dst.rows);
			cv::Mat roi(dst, rect);
			//�؂�o�����摜��1�F�œh��Ԃ�
			roi = colorHorizontal;
		}
	}
}