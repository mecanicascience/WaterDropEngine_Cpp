#pragma once

namespace wde {
	/**
	 * Class that removes the copy constructor and operator from derived class, and add a default constructor and destructor
	 */
	class NonCopyable {
		public:
			NonCopyable(const NonCopyable &) = delete;
			NonCopyable(NonCopyable &&) noexcept = delete;
			NonCopyable &operator=(const NonCopyable &) = delete;
			NonCopyable &operator=(NonCopyable &&) noexcept = delete;


		protected:
            explicit NonCopyable() = default;
			virtual ~NonCopyable() = default;
	};
}
