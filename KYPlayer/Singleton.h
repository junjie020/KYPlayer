#ifndef _SINGLETON_H_
#define _SINGLETON_H_
namespace KY
{
	template<class Type>
	class Singleton
	{
	public:
		Singleton()
		{
			BOOST_ASSERT(nullptr == ms_Inst);
			ms_Inst = static_cast<Type*>(this);
		}

		~Singleton()
		{
			ms_Inst = nullptr;
		}

		static Type* Create()
		{
#ifdef _DEBUG
			Type* pTmp = new Type;
			BOOST_ASSERT(pTmp == ms_Inst);
			return ms_Inst;
#else //!_DEBUG
			return new Type();
#endif // _DEBUG

		}

		static void Destory()
		{
			if (nullptr != ms_Inst)
			{
				delete ms_Inst;
				ms_Inst = nullptr;
			}
		}

		static Type* Inst()
		{
			return ms_Inst;
		}

	private:
		static Type* ms_Inst;
	};

	template<class Type>
	Type* Singleton<Type>::ms_Inst = nullptr;
}
#endif //_SINGLETON_H_