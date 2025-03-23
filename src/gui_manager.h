// gui_manager.h
#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include <algorithm>   // For std::find_if
#include <cstdio>      // For printf and similar functions
#include <functional>  // For std::function
#include <memory>      // For smart pointers
#include <vector>

// Rect helper
class Rect {
public:
    int x_, y_, width_, height_;
    Rect(int x, int y, int width, int height)
        : x_(x), y_(y), width_(width), height_(height) {
    }
    bool contains(int x, int y) const {
        return x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + height_;
    }

    int x() const {
        return x_;
    }
    int y() const {
        return y_;
    }
    int width() const {
        return width_;
    }
    int height() const {
        return height_;
    }
};

// Forward declaration
class Component;

#define EVT_CLICK 100
#define EVT_NOTIFY 101

class Handler {
public:
    virtual void handle(Component* src, int evtCode, u_int32_t hparam,
                        u_int32_t lparam) {
    }
};

// Component base class
class Component : public Handler {
public:
    Component(int x, int y, int width, int height) : r(x, y, width, height) {
    }

    virtual ~Component() {
    }

    virtual void render() {
        onRender();
        for (Component* child : getChildren()) {
            child->render();
        }
    }

    virtual void hide() {
        for (Component* child : getChildren()) {
            child->hide();
        }
        setHiddenFlag(true);
        onHide();
    }

    virtual void onHide() {
    }

    bool contains(int x, int y) const {
        return r.contains(x, y);
    }

    virtual void onRender() = 0;

    bool click(int x, int y) {
        if (contains(x, y)) {
            for (Component* child : getChildren()) {
                if (child->click(x, y)) {
                    return true;
                }
            }
            notifyHandlers(EVT_CLICK, x, y);
            return onClick(x, y);
        }
        return false;
    }

    virtual bool onClick(
        int x, int y) = 0;  // Executes click action, returns true if handled

    int getX() const {
        return r.x();
    }
    int getY() const {
        return r.y();
    }
    int getWidth() const {
        return r.width();
    }
    int getHeight() const {
        return r.height();
    }
    Component* getParent() const {
        return parent_;
    }

    Component* add(Component* child) {
        children_.push_back(child);
        child->parent_ = this;
        return this;
    }

    Component* remove(Component* child) {
        auto it =
            std::find_if(children_.begin(), children_.end(),
                         [child](const Component* c) { return c == child; });
        if (it != children_.end()) {
            children_.erase(it);
            delete child;
        }
        return this;
    }

    std::vector<Component*> getChildren() {
        return children_;
    }

    bool getHiddenFlag() const {
        return hiddenFlag;
    }

    void setHiddenFlag(bool hidden) {
        hiddenFlag = hidden;
    }

    void addHandler(Handler* handler) {
        handlers.push_back(handler);
    }

    void notifyHandlers(int evtCode, u_int32_t hparam, u_int32_t lparam) {
        for (auto& handler : handlers) {
            handler->handle(this, evtCode, hparam, lparam);
        }
    }

private:
    Rect r;
    Component* parent_ = nullptr;  // Parent component
    std::vector<Component*> children_;
    std::list<Handler*> handlers;

    // set when the component was hidden
    bool hiddenFlag = true;
};

// Page Class
class Page : public Component {
public:
    Page(const std::string& name, int x, int y, int width, int height)
        : Component(x, y, width, height), name_(name) {
    }
    ~Page() override {
        for (auto& component : getChildren()) {
            delete component;
        }
    }

    void onRender() override {
    }

    bool onClick(int x, int y) override {
        return false;
    }

    const std::string& getName() const {
        return name_;
    }

private:
    std::string name_;
};

// UI Class
class UI {
public:
    static UI* getInstance() {
        static UI instance;
        return &instance;
    }

    UI* addPage(Page* page) {
        pages_.push_back(page);
        return this;
    }

    void nextPage() {
        if (active_page_ != -1)
            pages_[active_page_]->hide();
        active_page_ = (active_page_ + 1) % pages_.size();
        redraw = true;
    }

    void prevPage() {
        if (active_page_ != -1)
            pages_[active_page_]->hide();

        active_page_ = (active_page_ - 1 + pages_.size()) % pages_.size();
        redraw = true;
    }

    int getActivePage() const {
        return active_page_;
    }

    UI* setActivePage(int page) {
        if (page < 0 || page >= pages_.size())
            throw std::out_of_range("Invalid page index");

        if (active_page_ != -1)
            pages_[active_page_]->hide();

        active_page_ = page;
        redraw = true;
        return this;
    }

    void render() {
        // must fully redraw the screen before rendering any pages
        if (redraw) {
            redraw = false;
            if (redrawCallback_ != nullptr) {
                redrawCallback_();
            }
        }

        if (active_page_ >= 0 && active_page_ < pages_.size()) {
            pages_[active_page_]->render();
        }
    }

    bool click(int x, int y) {
        if (active_page_ >= 0 && active_page_ < pages_.size())
            return pages_[active_page_]->click(x, y);
        return false;
    }

    void redrawCallback(void (*callback)()) {
        redrawCallback_ = callback;
    }

private:
    void (*redrawCallback_)() = nullptr;

    UI() {
    }

    ~UI() {
        for (auto& page : pages_) {
            delete page;
        }
    }
    std::vector<Page*> pages_;
    int active_page_ = -1;
    bool redraw = false;
};

#endif  // GUI_MANAGER_H
