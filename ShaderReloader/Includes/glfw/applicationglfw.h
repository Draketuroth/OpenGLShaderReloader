#ifndef GLFW_APPLICATION_H
#define GLFW_APPLICATION_H

#include "glfwbase.h"

namespace glfw 
{
	const enum EventType : char
	{
		KEY_PRESSED,

		MOUSE_ENTERED,
		MOUSE_LEAVED,
		MOUSE_PRESSED,

		WINDOW_RESIZE,
		WINDOW_REFRESH,
		WINDOW_CLOSE,
		WINDOW_FOCUS
	};

	const enum KeyType : char
	{
		A, S, D, W, Q, E, R, T, ENTER,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};

	class Event
	{
	public:

		EventType getEventType()
		{
			return _type;
		}
		Window* getEventSource()
		{
			return _source;
		}
		~Event()
		{
			std::cout << "Deleting event" << std::endl;
		}

	protected:
		EventType _type;
		Window* _source;
	};

	class MouseEvent : public Event
	{
	public:

		MouseEvent(Window* w, EventType mouseEventType)
		{
			_source = w;
			_type = EventType::MOUSE_ENTERED;
		}
	};

	class KeyEvent : public Event
	{
	public:
		KeyEvent(Window* w, EventType keyEventType, KeyType pressed_key)
		{
			_source = w;
			_type = keyEventType;
			_keyType = pressed_key;
		}
		KeyType getPressedKey()
		{
			return _keyType;
		}

	protected:
		KeyType _keyType;
	};

	class WindowEvent : public Event
	{
	public:
		WindowEvent(Window* w, EventType windowEventType)
		{
			_source = w;
			_type = windowEventType;
		}
		WindowEvent(Window* w, int focus)
		{
			_source = w;
			_type = EventType::WINDOW_FOCUS;
			_focus = focus;
		}
		WindowEvent(Window* w, int width, int height)
		{
			_source = w;
			_type = EventType::WINDOW_RESIZE;
			_width = width;
			_height = height;
		}
		~WindowEvent()
		{
			std::cout << "deleting window event" << std::endl;
		}

		int getWidth()
		{
			return _width;
		}
		int getHeight()
		{
			return _height;
		}
		int getFocus()
		{
			return _focus;
		}

	private:
		int _width;
		int _height;
		int _focus;
	};

	template <class T>
	class EventHandler
	{
	public:
		std::function<void(T* e)> handle;
		EventHandler(std::function<void(T* e)> func)
		{
			handle = func;
		}
	};

	class Window
	{
	public:
		Window(int width, int height, const char* title, GLFWmonitor* monitor)
		{
			_window = glfwCreateWindow(width, height, title, monitor, NULL);
			_ptr->w = this;
			_ptr->userPointer = NULL;
			glfwSetWindowUserPointer(_window, _ptr);
			setCallbacks();
		}
		~Window()
		{
			delete _ptr;
			resizeHandlers.clear();
			closeHandlers.clear();
			focusHandlers.clear();
		}

		void setOnWindowResized(EventHandler<class WindowEvent>* handler)
		{
			resizeHandlers.clear();
			resizeHandlers.push_back(handler);
		}

		void setOnWindowCloseRequest(EventHandler<class WindowEvent>* handler)
		{
			closeHandlers.clear();
			closeHandlers.push_back(handler);
		}

		void show()
		{
			glfwShowWindow(_window);
		}

		void hide()
		{
			glfwHideWindow(_window);
		}

		int isDestroyed()
		{
			return glfwWindowShouldClose(_window);
		}

		void makeContextCurrent()
		{
			glfwMakeContextCurrent(_window);
		}

		void swapBuffers()
		{
			glfwSwapBuffers(_window);
		}

		void destroy()
		{
			glfwDestroyWindow(_window);
		}

		static void pollEvents()
		{
			glfwPollEvents();
		}

		void* getUserPointer()
		{
			return _ptr->userPointer;
		}

		void setUserPointer(void* userPointer)
		{
			_ptr->userPointer = userPointer;
		}

	private:
		struct UserPointer
		{
			Window* w;
			void* userPointer;
		};

		static void windowSizeCallback(GLFWwindow* w, int width, int height)
		{
			((UserPointer*)glfwGetWindowUserPointer(w))->w->resizeCallback(w, width, height);
		}
		static void windowCloseCallback(GLFWwindow* w)
		{
			((UserPointer*)glfwGetWindowUserPointer(w))->w->closeCallback(w);
		}

		static void windowFocusCallback(GLFWwindow* w, int arg)
		{
			((UserPointer*)glfwGetWindowUserPointer(w))->w->focusCallback(w, arg);
		}

		// static void windowIconifyCallback(GLFWwindow* w, int arg) {}
		// static void windowPosCallback(GLFWwindow* w, int x, int y) {}
		// static void windowRefresh(GLFWwindow* w) {}

		void resizeCallback(GLFWwindow* w, int width, int height)
		{
			std::unique_ptr<WindowEvent> event(new WindowEvent(this, width, height));
			for (const auto handler : resizeHandlers)
			{
				handler->handle(event.get());
			}
			WindowEvent* ptr = event.release();
			delete ptr;
		}

		void closeCallback(GLFWwindow* w)
		{
			std::cout << "CLOSING" << std::endl;
			std::unique_ptr<WindowEvent> event(new WindowEvent(this, EventType::WINDOW_CLOSE));
			for (const auto handler : closeHandlers)
			{
				handler->handle(event.get());
			}
			WindowEvent* ptr = event.release();
			delete ptr;
		}

		std::function<void(GLFWwindow*, int)> focusCallback = std::function<void(GLFWwindow*, int)>
			([&](GLFWwindow * w, int focus) {

			std::unique_ptr<WindowEvent> event(new WindowEvent(this, focus));
			for (const auto handler : focusHandlers)
			{
				handler->handle(event.get());
			}
			WindowEvent* ptr = event.release();
			delete ptr;
				});

		void setCallbacks()
		{
			glfwSetWindowSizeCallback(_window, windowSizeCallback);
			glfwSetWindowCloseCallback(_window, windowCloseCallback);
			glfwSetWindowFocusCallback(_window, windowFocusCallback);
		}

	private:
		UserPointer* _ptr = new UserPointer();
		GLFWwindow* _window;
		std::vector<const EventHandler<class WindowEvent>*> resizeHandlers, closeHandlers, focusHandlers;
	};

	static bool init() 
	{
		return glfwInit();
	}

	static void terminate() 
	{
		glfwTerminate();
	}
};

#endif