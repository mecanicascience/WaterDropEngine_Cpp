#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "../WdeUtils/NonCopyable.hpp"


// Creates logs into logs/ that can be viewed in chrome://tracing/
namespace wde {
	using floatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult {
		std::string name;
		floatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession {
		std::string name;
	};


	class Instrumentor : public NonCopyable {
		public:
			void beginSession(const std::string& name, const std::string& filepath = "results.json") {
				std::lock_guard lock(_mutex);
				if (_currentSession) {
					Logger::err("Cannot begin Instrumentor session when another is already open.", LoggerChannel::MAIN);
					internalEndSession();
				}
				_outputStream.open(filepath);
				if (_outputStream.is_open()) {
					_currentSession = new InstrumentationSession({name});
					writeHeader();
				}
				else
					Logger::err("Error opening profiler results file.", LoggerChannel::MAIN);
			}

			void endSession() {
				std::lock_guard lock(_mutex);
				internalEndSession();
			}


			void writeProfile(const ProfileResult& result) {
				std::stringstream json;

				json << std::setprecision(3) << std::fixed;
				json << ",{";
				json << R"("cat":"function",)";
				json << "\"dur\":" << (result.elapsedTime.count()) << ',';
				json << R"("name":")" << result.name << "\",";
				json << R"("ph":"X",)";
				json << "\"pid\":0,";
				json << "\"tid\":" << result.threadID << ",";
				json << "\"ts\":" << result.start.count();
				json << "}";

				std::lock_guard lock(_mutex);
				if (_currentSession) {
					_outputStream << json.str();
					_outputStream.flush();
				}
			}


			static Instrumentor& get() {
				static Instrumentor instance;
				return instance;
			}


		private:
			InstrumentationSession* _currentSession;
			std::ofstream _outputStream;
			std::mutex _mutex;

            explicit Instrumentor() : _currentSession(nullptr) { }
			~Instrumentor() { endSession(); }

			void writeHeader() {
				_outputStream << R"({"otherData": {},"traceEvents":[{})";
				_outputStream.flush();
			}

			void writeFooter() {
				_outputStream << "]}";
				_outputStream.flush();
			}

			void internalEndSession() {
				if (_currentSession) {
					writeFooter();
					_outputStream.close();
					delete _currentSession;
					_currentSession = nullptr;
				}
			}
	};

	class InstrumentationTimer {
		public:
			explicit InstrumentationTimer(const char* name) : _name(name), _stopped(false) {
				_startTime = std::chrono::steady_clock::now();
			}

			~InstrumentationTimer() {
				if (!_stopped)
					stop();
			}

			void stop() {
				auto endTime = std::chrono::steady_clock::now();
				auto highResStart = floatingPointMicroseconds { _startTime.time_since_epoch() };
				auto elapsedTime =
						  std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch()
						- std::chrono::time_point_cast<std::chrono::microseconds>(_startTime).time_since_epoch();

				Instrumentor::get().writeProfile({ _name, highResStart, elapsedTime, std::this_thread::get_id() });

				_stopped = true;
			}

		private:
			const char* _name;
			std::chrono::time_point<std::chrono::steady_clock> _startTime;
			bool _stopped;
	};



	namespace instrumentorUtils {
		template <size_t N>
		struct ChangeResult {
			char data[N];
		};

		template <size_t N, size_t K>
		constexpr auto cleanupOutputString(const char(&expr)[N], const char(&remove)[K]) {
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N) {
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}


	class Instrumentation {
		public:
            explicit Instrumentation(const char* name) {
				#ifdef WDE_USE_PROFILING
					_timerSession = new InstrumentationTimer(name);
				#endif
			}

			~Instrumentation() {
				#ifdef WDE_USE_PROFILING
					delete _timerSession;
					_timerSession = nullptr;
				#endif
			}

		private:
			#ifdef WDE_USE_PROFILING
				InstrumentationTimer* _timerSession;
			#endif
	};
}



// Defines profiler options
#ifdef WDE_USE_PROFILING
	#define WDE_PROFILE_BEGIN_SESSION(name, filepath) Instrumentor::get().beginSession(name, filepath)
	#define WDE_PROFILE_END_SESSION() Instrumentor::get().endSession()

	#define WDE_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = instrumentorUtils::cleanupOutputString(name, "__cdecl ");\
												   InstrumentationTimer timer##line(fixedName##line.data)
	#define WDE_PROFILE_SCOPE_LINE(name, line) WDE_PROFILE_SCOPE_LINE2(name, line)
	#define WDE_PROFILE_SCOPE(name) WDE_PROFILE_SCOPE_LINE(name, __LINE__)
	#define WDE_PROFILE_FUNCTION() WDE_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
	#define WDE_PROFILE_BEGIN_SESSION(name, filepath)
	#define WDE_PROFILE_END_SESSION()

	#define WDE_PROFILE_SCOPE(name)
	#define WDE_PROFILE_FUNCTION()
#endif

