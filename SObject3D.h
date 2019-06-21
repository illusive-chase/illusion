#pragma once

#include "SPointer.h"
#include "SGeomMath.h"
#include "Surface.h"
#include "PObject3D.h"

namespace fl {
	namespace geom {

		/// @brief 3D对象类，由Stage3D负责析构，储存所有渲染所需的信息，提供接口函数进行几何变换、顶点和面的添加
		///
		class SObject3D :public AutoPtr {
		public:
			Vector3D pos, m_x, m_y, m_z;
			vector<Vector3D> vertex;
			vector<Vector3D> normal;
			vector<Surface3D> surface;
			fl::physics::PObject3D* pobj;

			SObject3D(const Vector3D& pos) :pos(pos), m_x(1, 0, 0), m_y(0, 1, 0), m_z(0, 0, 1), pobj(nullptr) {}
			virtual ~SObject3D() {}

			/// @brief 添加顶点和顶点的法向量，返回在顶点列表中的序号
			/// @param[in] p 顶点
			/// @param[in] n 顶点法向量
			/// @return int
			int addPoint(const Vector3D& p, const Vector3D& n);

			/// @brief 添加顶点，返回在顶点列表中的序号
			/// @note 缺省的顶点法向量由原点指向顶点的向量代替，即n = p
			/// @param[in] p 顶点
			/// @return int
			int addPoint(const Vector3D& p);

			/// @brief 根据指定的顶点序号和纹理添加三角形面，返回在面列表中的序号
			/// @note UV坐标的缺省值是UV(0, 0)；顶点应以面的法向量指向观察者时的顺时针顺序加入
			/// @param[in] pa 顶点A的序号
			/// @param[in] pb 顶点B的序号
			/// @param[in] pc 顶点C的序号
			/// @param[in] texture 面的纹理
			/// @param[in] uva 顶点A的UV坐标，使用UV宏UV(u, v)表示
			/// @param[in] uvb 顶点B的UV坐标，使用UV宏UV(u, v)表示
			/// @param[in] uvc 顶点C的UV坐标，使用UV宏UV(u, v)表示
			/// @return int
			int addSurface(int pa, int pb, int pc, const Texture& texture, int uva = 0, int uvb = 0, int uvc = 0);

			/// @brief 添加物理实体
			/// @note 物理实体的缺省值为空
			/// @param[in] obj 物理实体
			/// @return void
			void addPObject(fl::physics::PObject3D* obj);


			void rotateX(const Rad& rad);//clockwise
			void rotateY(const Rad& rad);
			void rotateZ(const Rad& rad);
			void scale(scalar factor);
			inline void move(const Vector3D& dir) { pos += dir; }
			void rotateX(const Rad& rad, const Vector3D& refv);//clockwise
			void rotateY(const Rad& rad, const Vector3D& refv);
			void rotateZ(const Rad& rad, const Vector3D& refv);
			void scale(scalar factor, const Vector3D& refv);

			/// @brief 每一逻辑帧都会被Stage3D调用
			/// @param void
			/// @return void
			virtual void framing() { if (pobj) pobj->framing(), pos = pobj->pos; };
			
		};

		//future: 加入可持久化功能
		/// @brief 3D对象容器类，本身并不是SObject对象，而是用于绑定SObject对象，然后调用接口函数统一对绑定对象操作
		/// @note 各种变换方法将等同地作用在所有绑定对象上
		class Sprite3D :public AutoPtr {
		public:
			list<SObject3D*> children;        ///< 绑定对象的指针，显然该类因为不是SObject对象，故并不负责绑定对象的析构

			Sprite3D() {}
			inline void addObject(SObject3D* p0) { children.push_back(p0); }
			inline void addObject(SObject3D* p0, SObject3D* p1) { addObject(p0); addObject(p1); }
			inline void addObject(SObject3D* p0, SObject3D* p1, SObject3D* p2) { addObject(p0, p1); addObject(p2); }
			inline void rotateX(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateX(rad, refv); }
			inline void rotateY(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateY(rad, refv); }
			inline void rotateZ(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateZ(rad, refv); }
			inline void scale(scalar factor, const Vector3D& refv) { for (SObject3D* it : children) it->scale(factor, refv); }
			inline void move(const Vector3D& dir) { for (SObject3D* it : children) it->move(dir); }
		};
	}
}