#pragma once
#include "SGeomMath.h"
#include <set>
#include <stack>

namespace fl {
	namespace physics {

		class OverlappingPairs {

			struct OP { 
				struct Less{
					bool operator()(const OP& lhs, const OP& rhs) const {
						if (lhs.a == rhs.a) return lhs.b < rhs.b;
						return lhs.a < rhs.a;
					}
				};
				struct Greater {
					bool operator()(const OP& lhs, const OP& rhs) const {
						if (lhs.a == rhs.a) return lhs.b > rhs.b;
						return lhs.a > rhs.a;
					}
				};
				union {
					struct { void* a, *b; };
					struct { size_t ia, ib; };
				};
				OP(void* x, void* y) {
					if (x < y) a = x, b = y;
					else a = y, b = x;
				}
			};
			

			std::set<OP, OP::Less> l_set;
			std::set<OP, OP::Greater> g_set;

		public:
			using OverlappingPair = OP;

			using iterator = std::set<OP, OP::Less>::iterator;

			ILL_INLINE const iterator begin() const { return l_set.begin(); }

			ILL_INLINE const iterator end() const { return l_set.end(); }

			ILL_INLINE void remove(void* obj) {
				OP p(0, 0);
				p.a = obj;
				for (auto it = l_set.lower_bound(p); it != l_set.end() && it->a == obj; it++) it = l_set.erase(it);
				p.ia++;
				for (auto it = g_set.upper_bound(p); it != g_set.end() && it->a == obj; it++) it = g_set.erase(it);
			}

			ILL_INLINE void add(void* oa, void* ob) {
				OP p(oa, ob);
				l_set.insert(p);
				g_set.insert(p);
			}

			ILL_INLINE void cleanup() {
				l_set.clear();
				g_set.clear();
			}

			ILL_INLINE bool contain(void* oa, void* ob) const {
				return l_set.find(OP(oa, ob)) != l_set.end();
			}
		};

		class BVH {
			using AxisAlignedBoundingBox = geom::AxisAlignedBoundingBox;

		public:

			ILL_ATTRIBUTE_ALIGNED16(class) Node {
			public:

				ILL_DECLARE_ALIGNED_ALLOCATOR

				AxisAlignedBoundingBox aabb;
				Node* pa;
				union {
					struct { Node* lc, *rc; };
					struct { Node* children[2]; };
					struct { size_t child; void* obj; };
				};

				ILL_INLINE bool is_leaf() const { return !child; }
				Node(const AxisAlignedBoundingBox& aabb, Node* pa) :aabb(aabb), pa(pa), lc(0), rc(0) {}
				Node(const AxisAlignedBoundingBox& aabb, void* obj, Node* pa) :aabb(aabb), obj(obj), pa(pa), child(0) {}
			};

			Node* m_root;
			Node* m_free;
			int m_leaves;
			int m_opath;

			BVH() :m_root(0), m_free(0), m_leaves(0), m_opath(0) {}

			ILL_INLINE Node* createLeafNode(const AxisAlignedBoundingBox& aabb, void* obj, Node* pa) {
				Node* ret;
				if (m_free) ret = m_free, m_free = 0, ret->aabb = aabb, ret->obj = obj, ret->child = 0, ret->pa = pa;
				else ret = new Node(aabb, obj, pa);
				return ret;
			}

			ILL_INLINE Node* createInternalNode(const AxisAlignedBoundingBox& aabb, Node* pa) {
				Node* ret;
				if (m_free) ret = m_free, m_free = 0, ret->aabb = aabb, ret->lc = 0, ret->rc = 0, ret->pa = pa;
				else ret = new Node(aabb, pa);
				return ret;
			}

			static ILL_INLINE int select(const AxisAlignedBoundingBox& o,
				const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b) 
			{
#ifdef ILL_SSE_IN_API
				static ILL_ATTRIBUTE_ALIGNED16(const unsigned) mask[] = { 0x7fffffff, 0x7fffffff, 0x7fffffff,  0x7fffffff };
				__m128 omi(_mm_load_ps(o.mi));
				omi = _mm_add_ps(omi, _mm_load_ps(o.mx));
				__m128 ami(_mm_load_ps(a.mi));
				ami = _mm_add_ps(ami, _mm_load_ps(a.mx));
				ami = _mm_sub_ps(ami, omi);
				ami = _mm_and_ps(ami, _mm_load_ps((const float*)mask));
				__m128 bmi(_mm_load_ps(b.mi));
				bmi = _mm_add_ps(bmi, _mm_load_ps(b.mx));
				bmi = _mm_sub_ps(bmi, omi);
				bmi = _mm_and_ps(bmi, _mm_load_ps((const float*)mask));
				__m128 t0(_mm_movehl_ps(ami, ami));
				ami = _mm_add_ps(ami, t0);
				ami = _mm_add_ss(ami, _mm_shuffle_ps(ami, ami, 1));
				__m128 t1(_mm_movehl_ps(bmi, bmi));
				bmi = _mm_add_ps(bmi, t1);
				bmi = _mm_add_ss(bmi, _mm_shuffle_ps(bmi, bmi, 1));
				union illSSEUnion
				{
					__m128 ssereg;
					float floats[4];
					int ints[4];
				} tmp;
				tmp.ssereg = _mm_cmple_ss(bmi, ami);
				return tmp.ints[0] & 1;
#else
				Vector3D doa = (o.mi + o.mx) - (a.mi + a.mx);
				Vector3D dob = (o.mi + o.mx) - (b.mi + b.mx);
				return (illFabs(doa.x) + illFabs(doa.y) + illFabs(doa.z) < illFabs(dob.x) + illFabs(dob.y) + illFabs(dob.z))
					? 0 : 1;
#endif
			}
			
			
			void insert(Node* leaf) {
				if (!m_root) {
					m_root = leaf;
					++m_leaves;
					return;
				}
				Node* root;
				for (root = m_root; !m_root->is_leaf();) {
					root = root->children[select(leaf->aabb, root->lc->aabb, root->rc->aabb)];
				}
				Node* pa = root->pa;
				Node* n_pa = createInternalNode(root->aabb, pa);
				root->pa = n_pa;
				n_pa->lc = root;
				n_pa->rc = leaf;
				leaf->pa = n_pa;
				if (pa) pa->children[pa->lc == root ? 0 : 1] = n_pa;
				else m_root = n_pa;
				for (; n_pa && n_pa->aabb.contain(leaf->aabb); n_pa = n_pa->pa) {
					n_pa->aabb.merge(leaf->aabb);
				}
				++m_leaves;
			}

			ILL_INLINE void release(Node* p) {
				if (m_free) delete m_free;
				m_free = p;
			}

			void remove(Node* leaf) {
				if (!leaf->pa) {
					m_root = 0;
					--m_leaves;
					return;
				}
				Node* pa = leaf->pa;
				Node* ppa = pa->pa;
				Node* bro = pa->children[pa->lc == leaf ? 1 : 0];
				bro->pa = ppa;
				if (ppa) ppa->children[ppa->lc == pa ? 0 : 1] = bro;
				else m_root = bro;
				leaf->pa = 0;
				release(pa);
				AxisAlignedBoundingBox temp;
				for (; ppa && ppa->aabb.merge(ppa->lc->aabb, ppa->rc->aabb); ppa = ppa->pa);
				--m_leaves;
			}

			void optimize(int passes) {
				if (passes < 0) passes = m_leaves;
				if (m_root) {
					while (passes--) {
						unsigned bit = 0;
						Node* root;
						for (root = m_root; !root->is_leaf(); ) {
							root = root->children[(m_opath >> bit) & 1];
							bit = (bit + 1) & 31;
						}
						update(root);
						++m_opath;
					}
				}
			}

			ILL_INLINE void update(Node* leaf) {
				remove(leaf);
				insert(leaf);
			}

			ILL_INLINE void remove_release(Node* leaf) {
				remove(leaf);
				release(leaf);
			}


			void collide(OverlappingPairs& op, Node* leaf) const {
				if (m_root) {
					std::stack<Node*> stk;
					stk.push(m_root);
					do {
						Node* n = stk.top();
						stk.pop();
						if (n->aabb.intersect(leaf->aabb)) {
							if (n->is_leaf()) {
								if (n != leaf && !op.contain(n->obj, leaf->obj)) op.add(n->obj, leaf->obj);
							} else stk.push(n->lc), stk.push(n->rc);
						}
					} while (!stk.empty());
				}
			}

			void collide(OverlappingPairs& op, std::stack<void*>& stk_obj, Node* leaf) const {
				if (m_root) {
					std::stack<Node*> stk;
					stk.push(m_root);
					do {
						Node* n = stk.top();
						stk.pop();
						if (n->aabb.intersect(leaf->aabb)) {
							if (n->is_leaf()) {
								if (n != leaf && !op.contain(n->obj, leaf->obj)) stk_obj.push(n->obj), op.add(n->obj, leaf->obj);
							} else stk.push(n->lc), stk.push(n->rc);
						}
					} while (!stk.empty());
				}
			}



		};

		class PBroadCollision {
		private:
			using AxisAlignedBoundingBox = geom::AxisAlignedBoundingBox;

		public:
			struct Proxy {
				BVH::Node* leaf;
				bool dynamic;
				Proxy(BVH::Node* leaf, bool dynamic) :leaf(leaf), dynamic(dynamic) {}
			};

			BVH static_bvh, dynamic_bvh;
			std::vector<Proxy> leaves;
			OverlappingPairs m_op;
			
			ILL_INLINE int createStaticProxy(const AxisAlignedBoundingBox& aabb, void* obj) {
				BVH::Node* leaf = static_bvh.createLeafNode(aabb, obj, 0);
				leaves.push_back(Proxy(leaf, false));
				static_bvh.collide(m_op, leaf);
				dynamic_bvh.collide(m_op, leaf);
				static_bvh.insert(leaf);
				return (int)leaves.size() - 1;
			}

			ILL_INLINE int createDynamicProxy(const AxisAlignedBoundingBox& aabb, void* obj) {
				BVH::Node* leaf = dynamic_bvh.createLeafNode(aabb, obj, 0);
				leaves.push_back(Proxy(leaf, true));
				static_bvh.collide(m_op, leaf);
				dynamic_bvh.collide(m_op, leaf);
				dynamic_bvh.insert(leaf);
				
				return (int)leaves.size() - 1;
			}

			ILL_INLINE int createStaticProxyInstant(std::stack<void*>& stk_obj, const AxisAlignedBoundingBox& aabb, void* obj) {
				BVH::Node* leaf = static_bvh.createLeafNode(aabb, obj, 0);
				leaves.push_back(Proxy(leaf, false));
				static_bvh.collide(m_op, stk_obj, leaf);
				dynamic_bvh.collide(m_op, stk_obj, leaf);
				static_bvh.insert(leaf);
				return (int)leaves.size() - 1;
			}

			ILL_INLINE int createDynamicProxyInstant(std::stack<void*>& stk_obj, const AxisAlignedBoundingBox& aabb, void* obj) {
				BVH::Node* leaf = dynamic_bvh.createLeafNode(aabb, obj, 0);
				leaves.push_back(Proxy(leaf, true));
				static_bvh.collide(m_op, stk_obj, leaf);
				dynamic_bvh.collide(m_op, stk_obj, leaf);
				dynamic_bvh.insert(leaf);

				return (int)leaves.size() - 1;
			}

			ILL_INLINE void destroyProxy(int index) {
				m_op.remove(leaves[index].leaf->obj);
				if (leaves[index].dynamic) dynamic_bvh.remove_release(leaves[index].leaf);
				else static_bvh.remove_release(leaves[index].leaf);
				leaves[index].leaf = 0;
			}

			ILL_INLINE void setProxy(int index, const AxisAlignedBoundingBox& aabb, const geom::Vector3D& vel) {
				BVH::Node* leaf = leaves[index].leaf;
#ifdef ILL_DEBUG
				ILLAssert(leaf->dynamic);
#endif
				leaf->aabb = aabb;
				leaf->aabb.expand(vel);
				dynamic_bvh.update(leaf);
				static_bvh.collide(m_op, leaf);
				dynamic_bvh.collide(m_op, leaf);
			}

			ILL_INLINE void setProxyInstant(
				std::stack<void*>& stk_obj,
				int index,
				const AxisAlignedBoundingBox& aabb,
				const geom::Vector3D& vel
			) {
				BVH::Node* leaf = leaves[index].leaf;
#ifdef ILL_DEBUG
				ILLAssert(leaf->dynamic);
#endif
				leaf->aabb = aabb;
				leaf->aabb.expand(vel);
				dynamic_bvh.update(leaf);
				static_bvh.collide(m_op, stk_obj, leaf);
				dynamic_bvh.collide(m_op, stk_obj, leaf);
			}

			ILL_INLINE void cleanup() { m_op.cleanup(); }
			
		};

		
#if 0
		class LBVH {
		public:

			class Node {
			public:
				AxisAlignedBoundingBox aabb;
				int pa;
				int depth;
				unsigned morton;
				union {
					struct { int lc, rc; };
					struct { int children[2]; };
					struct { int leaf, data; };
				};

				Node() {}
				Node(const AxisAlignedBoundingBox& aabb, int pa, int depth, int morton, int lc, int rc)
					:aabb(aabb), pa(pa), depth(depth), morton(morton), lc(lc), rc(rc) 
				{
				}
			};

			static constexpr int offset = 10;

			std::vector<Node> m_nodes;
			std::vector<int> m_leaves;
			int m_root;

			const AxisAlignedBoundingBox m_valid_range;

			LBVH(const AxisAlignedBoundingBox& m_valid_range) : m_valid_range(m_valid_range), m_root(-1) {}

			ILL_INLINE unsigned getMortonCode(const AxisAlignedBoundingBox& aabb) {
				Vector3D xyz = (aabb.center() - m_valid_range.mi) / (m_valid_range.size()) * scalar(1 << offset);
				unsigned x = (unsigned)xyz.x;
				unsigned y = (unsigned)xyz.y;
				unsigned z = (unsigned)xyz.z;

				x = (x | (x << 16)) & 0x030000FF;
				x = (x | (x << 8)) & 0x0300F00F;
				x = (x | (x << 4)) & 0x030C30C3;
				x = (x | (x << 2)) & 0x09249249;

				y = (y | (y << 16)) & 0x030000FF;
				y = (y | (y << 8)) & 0x0300F00F;
				y = (y | (y << 4)) & 0x030C30C3;
				y = (y | (y << 2)) & 0x09249249;

				z = (z | (z << 16)) & 0x030000FF;
				z = (z | (z << 8)) & 0x0300F00F;
				z = (z | (z << 4)) & 0x030C30C3;
				z = (z | (z << 2)) & 0x09249249;

				return x | (y << 1) | (z << 2);
			}

			ILL_INLINE int createNode(const Node& node) { m_nodes.push_back(node); return (int)m_nodes.size() - 1; }

			template<typename O>
			void insert(O* obj, int index) {

				while (index >= (int)m_leaves.size()) m_leaves.push_back(-1);

				unsigned morton = getMortonCode(obj->aabb);
				if (~m_root) {
					int place = m_root;
					unsigned p_morton = m_nodes[place].morton;
					int depth = 0;
					int mask = 1 << offset;
					bool find = false;
					int dir = 0;

					while (1) {
						while (depth < m_nodes[place].depth) {
							if (morton & mask != p_morton & mask) {
								find = true;
								break;
							}
							mask >>= 1;
							depth++;
						}

						int temp_dir = (morton & mask) ? 1 : 0;
						dir |= temp_dir << depth;

						if (find || (!~m_nodes[place].leaf)) break;

						place = m_nodes[place].children[temp_dir];
					}

					// create new node as place's brother node

					int pa = m_nodes[place].pa;
					int n_dir = (dir >> depth) & 1;
					Node n_Node(m_nodes[place].aabb, pa, depth, morton, 0, 0);
					n_Node.children[1 - n_dir] = place;
					int n_node = createNode(n_Node);
					if (~pa) m_nodes[pa].children[(dir >> m_nodes[pa].depth) & 1] = n_node;
					else m_root = n_node;
					m_nodes[place].pa = n_node;

					Node n_insert(obj->aabb, n_node, offset, morton, -1, index);
					m_leaves[index] = m_nodes[n_node].children[n_dir] = createNode(n_insert);
					updateAABB(n_node);
				} else m_leaves[index] = m_root = createNode(Node(obj->aabb, m_root, offset, morton, -1, index));
			}



		};
#endif

	}
}