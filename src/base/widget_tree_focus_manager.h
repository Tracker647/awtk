#ifndef WIDGET_TREE_FOCUS_MANAGER_H
#define WIDGET_TREE_FOCUS_MANAGER_H
#include "tkc/darray.h"
#include "base/widget.h"
BEGIN_C_DECLS
typedef enum {
    kNoChildrenFocusable = 0,
    kNoParentsFocusable = 1
} enWidgetTreeFocusEvent;

typedef ret_t (*widget_tree_focus_cb_t)(void *ctx, event_t *e);
typedef struct{
    widget_tree_focus_cb_t cb;
    void *ctx;
    event_t *e;
} widget_tree_focus_cb_info_t;

/**
 * @brief 当窗口基于控件树的"层级逻辑"进行焦点切换时，采用此类进行管理
 * 该类非单例，每个窗口对象都有一个这样的widget_tree_focus_manager_t对象
 * （如果切换到子控件，focusable控件将切换为子控件内部的所有的focusable控件，并屏蔽外部父层及以上的focusable控件）
 * 在xml上的使用方法例：
 * <window name="focus_test_window" style:normal:bg_color="#1A2F4A" move_focus_up_key="up" move_focus_down_key="down" move_focus_left_key="left" move_focus_right_key="right" move_focus_children_key="RETURN" move_focus_parent_key="ESCAPE">
 *  ...
 </window>
 * 
 * 可在窗口代码中设置无父级可聚焦焦点/无子级可聚焦焦点的回调：
 * ```
 * ret_t setting_menu_on_no_children_focusable(void *ctx, event_t *e){
 * 	printf("setting menu on no children focusable\r\n");
 * 	return RET_OK;
 * }
 * 
 * ret_t setting_menu_on_no_parents_focusable(void *ctx, event_t *e){
 * 	printf("setting menu on no parents focusable\r\n");
 * 	return RET_OK;
 * }
 * 
 * ret_t setting_menu_init(widget_t *win, void *ctx)
 * {
 * 	return_value_if_fail(win != NULL, RET_BAD_PARAMS);
 * 	widget_foreach(win, visit_init_child, win);
 * 	widget_tree_focus_set_cb(win, kNoChildrenFocusable, setting_menu_on_no_children_focusable, NULL, NULL);
 * 	widget_tree_focus_set_cb(win, kNoParentsFocusable, setting_menu_on_no_parents_focusable, NULL, NULL);
 * 	printf("setting_menu_init\r\n");
 * 	return RET_OK;
 * }
 * 
 *  ```
 */
typedef struct {
    widget_t *win;
    /**
     * @brief 无可聚焦的子控件时的回调管理
     * 
     */
    widget_tree_focus_cb_info_t on_no_children_focusable;
    /**
     * @brief 无可聚焦的父控件时的回调管理
     * 
     */
    widget_tree_focus_cb_info_t on_no_parents_focusable;

    /* private */
    /**
     * @brief 进入子控件层时，将上层开始的所有focusable控件list压栈到此
     * 
     */
    darray_t *g_focus_widget_list_stack;
    /**
     * @brief 进入子控件层时，将前一个聚焦的控件的压栈至此
     * 
     */
    darray_t *g_focus_widget_stack;


} widget_tree_focus_manager_t;

/* 用户函数 */
/**
 * @brief 设置控件树焦点回调，目前支持无父级控件可聚焦/无子级控件可聚焦两个事件
 * 
 * @param manager 
 * @param event 
 * @param cb 
 * @param ctx 
 * @param e 
 * @return ret_t 
 */
ret_t widget_tree_focus_set_cb(widget_t *win, enWidgetTreeFocusEvent event, widget_tree_focus_cb_t cb, void *ctx, event_t *e);

/* 以下函数仅在window_base.c使用 */
/**
 * @brief 创建widget_tree_focus_manager对象，仅在window_base.c中调用，别在用户程序使用！
 * 
 * @param win 
 * @return widget_tree_focus_manager_t* 
 */
widget_tree_focus_manager_t* widget_tree_focus_manager_create(widget_t *win);

/**
 * @brief 销毁widget_tree_focus_manager对象，仅在window_base.c中调用，别在用户程序使用！
 * 
 * @param win 
 * @return widget_tree_focus_manager_t* 
 */
ret_t widget_tree_focus_manager_destroy(widget_tree_focus_manager_t *manager);

/**
 * @brief 从窗口获取widget_tree_focus_manager对象，仅在window_base.c中调用，别在用户程序使用！
 * 
 * @param win 
 * @return widget_tree_focus_manager_t* 
 */
widget_tree_focus_manager_t* widget_tree_focus_manager_get_from_window(widget_t *win);


/**
 * @brief 移动焦点到上一级父控件，仅在window_base.c中调用，别在用户程序使用！
 * 
 * @param win 
 * @return widget_tree_focus_manager_t* 
 */
ret_t widget_tree_focus_move_parent(widget_tree_focus_manager_t* manager, widget_t *focused_widget);


/**
 * @brief 移动焦点到第一个子控件，仅在window_base.c中调用，别在用户程序使用！
 *  WAITFIX：将可激活的控件（比如按钮）区分出来处理，避免向内聚焦这些控件被激活
 * @param win 
 * @return widget_tree_focus_manager_t* 
 */
ret_t widget_tree_focus_move_children(widget_tree_focus_manager_t* manager, widget_t *focused_widget);


END_C_DECLS
#endif  /* WIDGET_TREE_FOCUS_MANAGER_H */
