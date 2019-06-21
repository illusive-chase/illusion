#pragma once

#include "SPointer.h"
#include "SGeomMath.h"
#include "Surface.h"
#include "PObject3D.h"

namespace fl {
	namespace geom {

		/// @brief 3D�����࣬��Stage3D��������������������Ⱦ�������Ϣ���ṩ�ӿں������м��α任�������������
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

			/// @brief ��Ӷ���Ͷ���ķ������������ڶ����б��е����
			/// @param[in] p ����
			/// @param[in] n ���㷨����
			/// @return int
			int addPoint(const Vector3D& p, const Vector3D& n);

			/// @brief ��Ӷ��㣬�����ڶ����б��е����
			/// @note ȱʡ�Ķ��㷨������ԭ��ָ�򶥵���������棬��n = p
			/// @param[in] p ����
			/// @return int
			int addPoint(const Vector3D& p);

			/// @brief ����ָ���Ķ�����ź���������������棬���������б��е����
			/// @note UV�����ȱʡֵ��UV(0, 0)������Ӧ����ķ�����ָ��۲���ʱ��˳ʱ��˳�����
			/// @param[in] pa ����A�����
			/// @param[in] pb ����B�����
			/// @param[in] pc ����C�����
			/// @param[in] texture �������
			/// @param[in] uva ����A��UV���꣬ʹ��UV��UV(u, v)��ʾ
			/// @param[in] uvb ����B��UV���꣬ʹ��UV��UV(u, v)��ʾ
			/// @param[in] uvc ����C��UV���꣬ʹ��UV��UV(u, v)��ʾ
			/// @return int
			int addSurface(int pa, int pb, int pc, const Texture& texture, int uva = 0, int uvb = 0, int uvc = 0);

			/// @brief �������ʵ��
			/// @note ����ʵ���ȱʡֵΪ��
			/// @param[in] obj ����ʵ��
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

			/// @brief ÿһ�߼�֡���ᱻStage3D����
			/// @param void
			/// @return void
			virtual void framing() { if (pobj) pobj->framing(), pos = pobj->pos; };
			
		};

		//future: ����ɳ־û�����
		/// @brief 3D���������࣬��������SObject���󣬶������ڰ�SObject����Ȼ����ýӿں���ͳһ�԰󶨶������
		/// @note ���ֱ任��������ͬ�����������а󶨶�����
		class Sprite3D :public AutoPtr {
		public:
			list<SObject3D*> children;        ///< �󶨶����ָ�룬��Ȼ������Ϊ����SObject���󣬹ʲ�������󶨶��������

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