#ifndef FUNCTIONAL_DEFINED
#define FUNCTIONAL_DEFINED

#pragma once

#include <type_traits>
#include <memory>
#include <functional>
#include <utility>

namespace Lib::Functional
{
	template <typename T>
	struct Success final
	{
	public:
		T value;
	};

	template <typename T>
	struct Error final
	{
	public:
		T value;
	};

	template <typename TSuccess, typename TError>
	struct Result final
	{
	private:
		struct SuccessTag {};
		struct ErrorTag {};

		union
		{
		public:
			TSuccess successValue;
			TError errorValue;
		};

		bool succeed;

		template <typename ...TParams>
		constexpr Result(SuccessTag, TParams &&...params) noexcept(noexcept(TSuccess(std::forward<TParams>(params...)))) :
			successValue(TSuccess(std::forward<TParams>(params...))), succeed(true) {}

		template <typename ...TParams>
		constexpr Result(ErrorTag, TParams &&...params) noexcept(noexcept(TError(std::forward<TParams>(params...)))) :
			errorValue(TError(std::forward<TParams>(params...))), succeed(false) {}

	public:
		constexpr Result(const Success<TSuccess> &value) noexcept(noexcept(TSuccess(std::move(value.value)))) :
			successValue(std::move(value.value)), succeed(true) {}

		constexpr Result(const Error<TError> &value) noexcept(noexcept(TError(std::move(value.value)))) :
			errorValue(std::move(value.value)), succeed(false) {}

		constexpr bool Succeed() const noexcept
		{
			return succeed;
		}

		constexpr const TSuccess &Get() const noexcept
		{
			return successValue;
		}

		constexpr TSuccess &Get() noexcept
		{
			return successValue;
		}

		constexpr const TError &GetError() const noexcept
		{
			return errorValue;
		}

		constexpr TError &GetError() noexcept
		{
			return errorValue;
		}

		template <typename ...TParams>
		constexpr Result Success(TParams &&...params) noexcept(noexcept(TSuccess(std::forward<TParams>(params...))))
		{
			return Result(SuccessTag{}, std::forward<TParams>(params...));
		}

		template <typename ...TParams>
		constexpr Result Error(TParams &&...params) noexcept(noexcept(TError(std::forward<TParams>(params...))))
		{
			return Result(ErrorTag{}, std::forward<TParams>(params...));
		}

		template <typename TOther, typename TFunc>
		constexpr Result<TOther, TError> Map(TFunc func) const noexcept(noexcept(TError(errorValue)))
		{
			return succeed ? Result<TOther, TError>::Success(func(successValue)) : Result<TOther, TError>::Error(errorValue);
		}

		template <typename TOther, typename TFunc>
		constexpr Result<TOther, TError> Map(TFunc func) noexcept(noexcept(TError(errorValue)))
		{
			return succeed ? Result<TOther, TError>::Success(func(successValue)) : Result<TOther, TError>::Error(errorValue);
		}

		constexpr explicit operator bool() const noexcept
		{
			return succeed;
		}

		constexpr ~Result() noexcept(noexcept(successValue.~TSuccess()) && noexcept(errorValue.~TError()))
		{
			if (succeed)
			{
				successValue.~TSuccess();
			}
			else
			{
				errorValue.~TError();
			}
		}
	};

	template <typename TResult, typename ...TParams>
	using FuncPtr = TResult(*)(TParams ...);

	template <typename TResult, typename TInstance, typename ...TParams>
	using InstanceMethod = TResult(TInstance:: *)(TParams...);

	template <typename TResult, typename ...TParams>
	class Delegate final
	{
	private:
		class IDelegate
		{
		public:
			TResult operator()(TParams ...params) = 0;

			virtual ~IDelegate() = default;
		};

		template <auto Invocable>
		struct CompileTimeInvocable final
		{
		public:
			TResult operator()(TParams ...params)
			{
				return Invocable(std::forward<TParams>(params)...);
			}
		};

		template <typename TLambda>
		struct InvocableLambda final
		{
		private:
			TLambda lambda;

		public:
			constexpr InvocableLambda(const TLambda &lambda) noexcept : lambda(lambda) {}
			constexpr InvocableLambda(TLambda &&lambda) noexcept : lambda(lambda) {}

			TResult operator()(TParams ...params)
			{
				return lambda(std::forward<TParams>(params)...);
			}
		};

		template <typename TInstance>
		struct MemberFunction final
		{
		private:
			TInstance instance;
			TResult(TInstance:: *method)(TParams ...);

		public:
			TResult operator()(TParams ...params)
			{
				return (instance.*method)(std::forward<TParams>(params)...);
			}
		};

		IDelegate *delegate;

	public:
		constexpr Delegate() noexcept = default;

		constexpr ~Delegate() noexcept
		{
			delete delegate;
		}
	};
}

#endif // !FUNCTIONAL_DEFINED
