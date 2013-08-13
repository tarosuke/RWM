/**************************************************** humanoid avatar:humanoid
 * AVATARのHUMANOID拡張
 */
#ifndef _HUMANOID_
#define _HUMANOID_

#include "avatar.h"


class HUMANOID : public AVATAR{
	HUMANOID();
	void operator=(AVATAR&);
public:
	/*** ヒューマノイドな体のスケルトンパラメタ
	 * legLength + torsoLengthがGetViewで作る高さ
	 * 首を傾けたり回したりすると首の位置を中心に視点が回転する
	 */
	class PROFILE{
	public:
		PROFILE(float tall,
			float terminalRatio = 1.35,
			float widthRatio = 1.0);
		//固定値
		static const float eyeSideOffset = 0.03;
		static const float eyeFrontOffset = 0.15;
		//デフォルトのバランス(胴の長さを1とする骨格上の比率)
		//末端係数一次
		static const float defaultHeadRatio = 0.225;
		static const float defaultNeckRatio = 0.07;
		static const float defaultLegRatio = 0.43; //upperLeg
		static const float defaultArmRatio = 0.37; //upperArm

		//末端係数二次
		static const float defaultCalfRatio = 0.6;
		static const float defaultLowerArmRatio = 0.6;

		//非末端
		static const float defaultSholderRatio = 0.15;
		static const float defaultWaistRatio = 0.12;

		/*** 頭
		 * 首の付け根が胴体の上端で、三軸関節扱い
		 */
		float armOffset; //肩のオフセット
		float legOffset; //脚の付け根のオフセット
		float eyeHeight; //目の高さ
		struct{
			struct{
				float length;
			}torso;
			struct{
				float height; //頭の高さ
				float neck; //首の長さ
			}head;
			struct{
				float upperLength;
				float lowerLength;
			}arm;
			struct{
				float upperLength;
				float lowerLength;
			}leg;
		}__attribute__((packed));;
		float width; //太さ係数
		float NeckHeight() const{
			return torso.length +
				leg.upperLength + leg.lowerLength;
		};
		float Tall() const{
			return NeckHeight() + head.neck + head.height;
		};
		float SightHeight() const{
			return NeckHeight() + head.neck + eyeHeight;
		};
	};

	//システムハンドラ
	void GetView() const;
	void Update(float dt);
	float Height() const{ return profile.SightHeight(); };
	//構築/破壊子
	HUMANOID(const class WORLD& world,
		const PROFILE&,
		class GHOST&);
	~HUMANOID();

protected:
	class POSTURE{
	public:
		POSTURE();
// 	protected:
		class LIMBS{
		public:
			LIMBS(double);
			class LIMB{
			public:
				LIMB(double);
				QON::ROTATION sh; //肩、股関節
				double en; //肘、膝
//TODO:				QON wh; //手首、足首
			}left, right;
		}arms, legs;
		QON::ROTATION upper; //上半身全体(体幹に対する相対角)
		QON::ROTATION lower; //下半身全体(体幹に対する相対角)
	}posture;
	PROFILE profile;
	bool sitting;
	void ArmVertexes(int) const;
	void LegVertexes(int) const;
	void DrawBody(const class TEXSET&) const;
	void DrawHead(const class TEXSET&) const;
};


#endif
