#ifndef WIDGET_TREE_FOCUS_MANAGER_H
#define WIDGET_TREE_FOCUS_MANAGER_H
#include "tkc/darray.h"
#include "base/widget.h"
BEGIN_C_DECLS
/**
 * @brief 当窗口基于控件树的"层级逻辑"进行焦点切换时，采用此类进行管理
 *  该类非单例，每个窗口对象都有一个这样的widget_tree_focus_manager_t对象
 * （如果切换到子控件，focusable控件将切换为子控件内部的所有的focusable控件，并屏蔽外部父层及以上的focusable控件）
 * 在xml上的使用方法例：
 * <window name="focus_test_window" style:normal:bg_color="#1A2F4A" move_focus_up_key="up" move_focus_down_key="down" move_focus_left_key="left" move_focus_right_key="right"  move_focus_children_key="RETURN" move_focus_parent_key="ESCAPE">
 ...
 </window>
 * 
 */
typedef struct {
    widget_t *win;
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

widget_tree_focus_manager_t* widget_tree_focus_manager_create(widget_t *win);
ret_t widget_tree_focus_move_parent(widget_tree_focus_manager_t* manager, widget_t *focused_widget);
ret_t widget_tree_focus_move_children(widget_tree_focus_manager_t* manager, widget_t *focused_widget);
ret_t widget_tree_focus_manager_destroy(widget_tree_focus_manager_t *manager);


END_C_DECLS
#endif  /* WIDGET_TREE_FOCUS_MANAGER_H */