#include "widget_tree_focus_manager.h"
#include "tkc/mem.h"
#include <stdbool.h>


static ret_t widget_print_all_focusable_widgets(darray_t *all_focusable)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			printf("widget_print_all_focusable_widgets>find widget name: %s at %#x\r\n", WIDGET(all_focusable->elms[i])->name, WIDGET(all_focusable->elms[i]));
		}
	}
	return RET_OK;
}

/**
 * @brief 查找以widget为根节点下的所有可聚焦子控件，记录在all_focusable数组上 
 * 
 * @param [in] widget 
 * @param [out] all_focusable 
 * @return ret_t 
 */
static ret_t widget_get_all_focusable_widgets(widget_t *widget, darray_t *all_focusable)
{
	widget_foreach(widget, widget_on_visit_focusable, all_focusable);
	return_value_if_fail(widget != NULL, RET_FAIL);
	return_value_if_fail(all_focusable != NULL && all_focusable->size > 0, RET_FAIL);
	return RET_OK;
}

static ret_t widget_set_all_focusable(darray_t *all_focusable, bool flag)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			widget_set_focusable(WIDGET(all_focusable->elms[i]), flag);
		}
	}
	return RET_OK;
}

static ret_t widget_set_all_focused(darray_t *all_focusable, bool flag)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			widget_set_prop_bool(WIDGET(all_focusable->elms[i]), WIDGET_PROP_FOCUSED, flag);
		}
	}
	return RET_OK;
}

widget_tree_focus_manager_t* widget_tree_focus_manager_create(widget_t *win){
    return_value_if_fail(win != NULL, NULL);
    widget_tree_focus_manager_t *manager = TKMEM_ZALLOC(widget_tree_focus_manager_t);
    manager->win = win;
    manager->g_focus_widget_list_stack = darray_create(10, NULL, NULL);
    manager->g_focus_widget_stack = darray_create(10, NULL, NULL);
    return manager;
}

ret_t widget_tree_focus_move_parent(widget_tree_focus_manager_t* manager, widget_t *focused_widget){
    return_value_if_fail(manager != NULL, RET_BAD_PARAMS);
    darray_t *g_focus_widget_list_stack = manager->g_focus_widget_list_stack;
    darray_t *g_focus_widget_stack = manager->g_focus_widget_stack;
    widget_t *win = manager->win;
    ret_t ret = RET_OK;
    if (g_focus_widget_list_stack->size == 0)
    {
        printf("no focusable parent widget\r\n");
        return RET_STOP;
    }
    /* 恢复父层控件的可聚焦性 */
    darray_t *parent_focusable_widget_list = darray_pop(g_focus_widget_list_stack);
    focused_widget = darray_pop(g_focus_widget_stack);
    ret = widget_set_all_focusable(parent_focusable_widget_list, TRUE);
    ret = widget_set_all_focused(parent_focusable_widget_list, FALSE);
    ret = widget_set_focused(focused_widget, TRUE);

    ret = darray_destroy(parent_focusable_widget_list);
    return ret;
}

ret_t widget_tree_focus_move_children(widget_tree_focus_manager_t* manager, widget_t *focused_widget){
    return_value_if_fail(manager != NULL, RET_BAD_PARAMS);
    darray_t *g_focus_widget_list_stack = manager->g_focus_widget_list_stack;
    darray_t *g_focus_widget_stack = manager->g_focus_widget_stack;
    widget_t *win = manager->win;
    ret_t ret = RET_OK;

    /* parent_focusable_widget_list的生命周期：在widget_tree_focus_move_children创建， 赋值入栈，在widget_tree_focus_move_parent出栈，销毁 */
    darray_t *parent_focusable_widget_list = darray_create(10, NULL, NULL);
    widget_get_all_focusable_widgets(win, parent_focusable_widget_list);
    
    /* child_focusable_widget_list的生命周期：比较简单，本函数内创建赋值，完成查找所有可聚焦子控件的任务后销毁 */
    darray_t *child_focusable_widget_list = darray_create(10, NULL, NULL);
    widget_get_all_focusable_widgets(focused_widget, child_focusable_widget_list);
    darray_remove(child_focusable_widget_list, focused_widget);

    if (child_focusable_widget_list->size > 0)
    {
        /* 屏蔽当前控件以上所有父层子控件，并聚焦当前控件的第一个子控件 */
        widget_set_all_focusable(parent_focusable_widget_list, FALSE);
        widget_set_all_focusable(child_focusable_widget_list, TRUE);
        widget_set_all_focused(parent_focusable_widget_list, FALSE);
        widget_set_focused(WIDGET(child_focusable_widget_list->elms[0]), TRUE);
        
        /* 保存这些取消聚焦的控件列表和前一个聚焦控件，以便在widget_tree_focus_move_parent中出栈恢复 */
        darray_push(g_focus_widget_list_stack, parent_focusable_widget_list);
        darray_push(g_focus_widget_stack, focused_widget);
    }
    else
    {
        printf("no focusable child widget\r\n");
    }
    ret = darray_destroy(child_focusable_widget_list);
    return ret;
}

ret_t widget_tree_focus_manager_destroy(widget_tree_focus_manager_t *manager)
{
    if(manager == NULL){
        return RET_OK;
    }
    darray_destroy(manager->g_focus_widget_stack);
    darray_destroy(manager->g_focus_widget_list_stack);
    TKMEM_FREE(manager);
    return RET_OK;
}