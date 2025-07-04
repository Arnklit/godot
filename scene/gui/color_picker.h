/**************************************************************************/
/*  color_picker.h                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/popup.h"

class AspectRatioContainer;
class ColorMode;
class ColorPickerShape;
class FileDialog;
class GridContainer;
class HSlider;
class Label;
class LineEdit;
class MarginContainer;
class MenuButton;
class OptionButton;
class PopupMenu;
class SpinBox;
class StyleBoxFlat;
class TextureRect;

class ColorPresetButton : public BaseButton {
	GDCLASS(ColorPresetButton, BaseButton);

	Color preset_color;
	bool recent = false;

	struct ThemeCache {
		Ref<StyleBox> foreground_style;
		Ref<StyleBox> focus_style;

		Ref<Texture2D> background_icon;
		Ref<Texture2D> overbright_indicator;
	} theme_cache;

protected:
	void _notification(int);
	static void _bind_methods();

public:
	void set_preset_color(const Color &p_color);
	Color get_preset_color() const;

	virtual String get_tooltip(const Point2 &p_pos) const override;

	ColorPresetButton(Color p_color, int p_size, bool p_recent);
	~ColorPresetButton();
};

class ColorPicker : public VBoxContainer {
	GDCLASS(ColorPicker, VBoxContainer);

	// These classes poke into theme items for their internal logic.
	friend class ColorPickerShape;
	friend class ColorPickerShapeRectangle;
	friend class ColorPickerShapeWheel;
	friend class ColorPickerShapeCircle;
	friend class ColorPickerShapeVHSCircle;
	friend class ColorPickerShapeOKHSLCircle;
	friend class ColorPickerShapeOKHSRectangle;
	friend class ColorPickerShapeOKHLRectangle;

	friend class ColorModeRGB;
	friend class ColorModeHSV;
	friend class ColorModeLinear;
	friend class ColorModeOKHSL;

public:
	enum ColorModeType {
		MODE_RGB,
		MODE_HSV,
#ifndef DISABLE_DEPRECATED
		MODE_RAW = 2,
#endif
		MODE_LINEAR = 2,
		MODE_OKHSL,

		MODE_MAX
	};

	enum PickerShapeType {
		SHAPE_HSV_RECTANGLE,
		SHAPE_HSV_WHEEL,
		SHAPE_VHS_CIRCLE,
		SHAPE_OKHSL_CIRCLE,
		SHAPE_NONE,
		SHAPE_OK_HS_RECTANGLE,
		SHAPE_OK_HL_RECTANGLE,

		SHAPE_MAX
	};

private:
	// Ideally, `SHAPE_NONE` should be -1 so that we don't need to convert shape type to index.
	// In order to avoid breaking compatibility, we have to use these methods for conversion.
	inline int get_current_shape_index() {
		return shape_to_index(current_shape);
	}

	static inline int shape_to_index(PickerShapeType p_shape) {
		if (p_shape == SHAPE_NONE) {
			return -1;
		}
		if (p_shape > SHAPE_NONE) {
			return p_shape - 1;
		}
		return p_shape;
	}

	static inline PickerShapeType index_to_shape(int p_index) {
		if (p_index == -1) {
			return SHAPE_NONE;
		}
		if (p_index >= SHAPE_NONE) {
			return (PickerShapeType)(p_index + 1);
		}
		return (PickerShapeType)p_index;
	}

public:
	static const int MODE_SLIDER_COUNT = 3;

	enum SLIDER_EXTRA {
		SLIDER_INTENSITY = MODE_SLIDER_COUNT,
		SLIDER_ALPHA,

		SLIDER_MAX
	};

	enum class MenuOption {
		MENU_SAVE,
		MENU_SAVE_AS,
		MENU_LOAD,
		MENU_QUICKLOAD,
		MENU_CLEAR,
	};

private:
	static inline Ref<Shader> wheel_shader;
	static inline Ref<Shader> circle_shader;
	static inline Ref<Shader> circle_ok_color_shader;
	static inline Ref<Shader> rectangle_ok_color_hs_shader;
	static inline Ref<Shader> rectangle_ok_color_hl_shader;
	static inline List<Color> preset_cache;
	static inline List<Color> recent_preset_cache;

#ifdef TOOLS_ENABLED
	Object *editor_settings = nullptr;
#endif

	int current_slider_count = MODE_SLIDER_COUNT;

	const float DEFAULT_GAMEPAD_EVENT_DELAY_MS = 1.0 / 2;
	const float GAMEPAD_EVENT_REPEAT_RATE_MS = 1.0 / 30;
	float gamepad_event_delay_ms = DEFAULT_GAMEPAD_EVENT_DELAY_MS;

	static constexpr int MODE_BUTTON_COUNT = 3;

	bool slider_theme_modified = true;

	LocalVector<ColorMode *> modes;
	LocalVector<ColorPickerShape *> shapes;

	Popup *picker_window = nullptr;
	TextureRect *picker_texture_zoom = nullptr;
	Panel *picker_preview = nullptr;
	Panel *picker_preview_color = nullptr;
	Ref<StyleBoxFlat> picker_preview_style_box;
	Ref<StyleBoxFlat> picker_preview_style_box_color;

	// Legacy color picking.
	TextureRect *picker_texture_rect = nullptr;
	Color picker_color;
	FileDialog *file_dialog = nullptr;
	MenuButton *menu_btn = nullptr;
	PopupMenu *options_menu = nullptr;

	MarginContainer *internal_margin = nullptr;
	HBoxContainer *shape_container = nullptr;
	TextureRect *sample = nullptr;
	VBoxContainer *swatches_vbc = nullptr;
	GridContainer *preset_container = nullptr;
	HBoxContainer *recent_preset_hbc = nullptr;
	Button *btn_add_preset = nullptr;
	Button *btn_pick = nullptr;
	Label *palette_name = nullptr;
	String palette_path;
	Button *btn_preset = nullptr;
	Button *btn_recent_preset = nullptr;
	PopupMenu *shape_popup = nullptr;
	PopupMenu *mode_popup = nullptr;
	MenuButton *btn_shape = nullptr;
	HBoxContainer *mode_hbc = nullptr;
	HBoxContainer *sample_hbc = nullptr;
	GridContainer *slider_gc = nullptr;
	HBoxContainer *hex_hbc = nullptr;
	Label *hex_label = nullptr;
	MenuButton *btn_mode = nullptr;
	Button *mode_btns[MODE_BUTTON_COUNT];
	Ref<ButtonGroup> mode_group;
	ColorPresetButton *selected_recent_preset = nullptr;
	Ref<ButtonGroup> preset_group;
	Ref<ButtonGroup> recent_preset_group;

	HBoxContainer *perm_hb = nullptr;
	void _req_permission();

#ifdef TOOLS_ENABLED
	Callable quick_open_callback;
	Callable palette_saved_callback;
#endif // TOOLS_ENABLED

	OptionButton *mode_option_button = nullptr;

	HSlider *sliders[SLIDER_MAX];
	SpinBox *values[SLIDER_MAX];
	Label *labels[SLIDER_MAX];

	Button *text_type = nullptr;
	LineEdit *c_text = nullptr;

	HSlider *alpha_slider = nullptr;
	SpinBox *alpha_value = nullptr;
	Label *alpha_label = nullptr;

	bool edit_alpha = true;

	HSlider *intensity_slider = nullptr;
	SpinBox *intensity_value = nullptr;
	Label *intensity_label = nullptr;

	bool edit_intensity = true;

	Size2i ms;
	bool text_is_constructor = false;
	PickerShapeType current_shape = SHAPE_HSV_RECTANGLE;
	ColorModeType current_mode = MODE_RGB;
	bool colorize_sliders = true;

	const int PRESET_COLUMN_COUNT = 9;
	int prev_preset_size = 0;
	int prev_rencet_preset_size = 0;
	List<Color> presets;
	List<Color> recent_presets;

	Color color;
	Color color_normalized;
	Color old_color;
	Color pre_picking_color;
	bool is_picking_color = false;

	bool display_old_color = false;
	bool deferred_mode_enabled = false;
	bool updating = true;
	bool changing_color = false;
	bool spinning = false;
	bool can_add_swatches = true;
	bool presets_visible = true;
	bool color_modes_visible = true;
	bool sampler_visible = true;
	bool sliders_visible = true;
	bool hex_visible = true;
	bool line_edit_mouse_release = false;
	bool text_changed = false;
	bool currently_dragging = false;

	float h = 0.0;
	float s = 0.0;
	float v = 0.0;

	float ok_hsl_h = 0.0;
	float ok_hsl_s = 0.0;
	float ok_hsl_l = 0.0;

	bool hsv_cached = false;
	bool okhsl_cached = false;

	float intensity = 0.0;

	struct ThemeCache {
		float base_scale = 1.0;

		int content_margin = 0;
		int label_width = 0;

		int sv_height = 0;
		int sv_width = 0;
		int h_width = 0;

		bool center_slider_grabbers = true;

		Ref<StyleBox> picker_focus_rectangle;
		Ref<StyleBox> picker_focus_circle;
		Color focused_not_editing_cursor_color;
		Ref<Texture2D> menu_option;
		Ref<Texture2D> screen_picker;
		Ref<Texture2D> expanded_arrow;
		Ref<Texture2D> folded_arrow;
		Ref<Texture2D> add_preset;

		Ref<Texture2D> shape_rect;
		Ref<Texture2D> shape_rect_wheel;
		Ref<Texture2D> shape_circle;

		Ref<Texture2D> bar_arrow;
		Ref<Texture2D> sample_bg;
		Ref<Texture2D> sample_revert;
		Ref<StyleBox> sample_focus;
		Ref<Texture2D> overbright_indicator;
		Ref<Texture2D> picker_cursor;
		Ref<Texture2D> picker_cursor_bg;
		Ref<Texture2D> color_hue;

		Ref<Texture2D> color_script;

		/* Mode buttons */
		Ref<StyleBox> mode_button_normal;
		Ref<StyleBox> mode_button_pressed;
		Ref<StyleBox> mode_button_hover;
	} theme_cache;

	void _copy_normalized_to_hsv_okhsl();
	void _copy_hsv_okhsl_to_normalized();

	Color _color_apply_intensity(const Color &col) const;
	void _normalized_apply_intensity_to_color();
	void _copy_color_to_normalized_and_intensity();

	void create_slider(GridContainer *gc, int idx);
	void _reset_sliders_theme();
	void _html_submitted(const String &p_html);
	void _slider_drag_started();
	void _slider_value_changed();
	void _slider_drag_ended();
	void _update_controls();
	void _update_color(bool p_update_sliders = true);
	void _update_text_value();
#ifdef TOOLS_ENABLED
	void _text_type_toggled();
#endif // TOOLS_ENABLED
	void _sample_input(const Ref<InputEvent> &p_event);
	void _sample_draw();
	void _slider_draw(int p_which);
	void _alpha_slider_draw();

	void _slider_or_spin_input(const Ref<InputEvent> &p_event);
	void _line_edit_input(const Ref<InputEvent> &p_event);
	void _preset_input(const Ref<InputEvent> &p_event, const Color &p_color);
	void _recent_preset_pressed(const bool pressed, ColorPresetButton *p_preset);
	void _text_changed(const String &p_new_text);
	void _add_preset_pressed();
	void _html_focus_exit();
	void _pick_button_pressed();
	void _target_gui_input(const Ref<InputEvent> &p_event);
	void _pick_finished();
	void _update_menu_items();
	void _options_menu_cbk(int p_which);
	void _block_input_on_popup_show();
	void _enable_input_on_popup_hide();

	// Native color picking.
	void _pick_button_pressed_native();
	void _native_cb(bool p_status, const Color &p_color);

	// Legacy color picking.
	void _pick_button_pressed_legacy();
	void _picker_texture_input(const Ref<InputEvent> &p_event);

	inline int _get_preset_size();
	void _add_preset_button(int p_size, const Color &p_color);
	void _add_recent_preset_button(int p_size, const Color &p_color);
	void _save_palette(bool p_is_save_as);
	void _load_palette();

	void _show_hide_preset(const bool &p_is_btn_pressed, Button *p_btn_preset, Container *p_preset_container);
	void _update_drop_down_arrow(const bool &p_is_btn_pressed, Button *p_btn_preset);

	void _set_mode_popup_value(ColorModeType p_mode);

	Variant _get_drag_data_fw(const Point2 &p_point, Control *p_from_control);
	bool _can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from_control) const;
	void _drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from_control);

	void _ensure_file_dialog();

protected:
	virtual void _update_theme_item_cache() override;

	void _notification(int);
	static void _bind_methods();

public:
#ifdef TOOLS_ENABLED
	void set_editor_settings(Object *p_editor_settings);
	void set_quick_open_callback(const Callable &p_file_selected);
	void set_palette_saved_callback(const Callable &p_palette_saved);

	void _quick_open_palette_file_selected(const String &p_path);
#endif

	GridContainer *get_slider_container();
	HSlider *get_slider(int idx);
	Vector<float> get_active_slider_values();

	static void init_shaders();
	static void finish_shaders();

	void add_mode(ColorMode *p_mode);
	void add_shape(ColorPickerShape *p_shape);

	void set_edit_alpha(bool p_show);
	bool is_editing_alpha() const;

	void set_edit_intensity(bool p_show);
	bool is_editing_intensity() const;

	void _set_pick_color(const Color &p_color, bool p_update_sliders, bool p_calc_intensity);
	void set_pick_color(const Color &p_color);
	Color get_pick_color() const;
	void set_old_color(const Color &p_color);
	Color get_old_color() const;

	void _palette_file_selected(const String &p_path);

	void set_display_old_color(bool p_enabled);
	bool is_displaying_old_color() const;

	void set_picker_shape(PickerShapeType p_shape);
	PickerShapeType get_picker_shape() const;

	void add_preset(const Color &p_color);
	void add_recent_preset(const Color &p_color);
	void erase_preset(const Color &p_color);
	void erase_recent_preset(const Color &p_color);
	PackedColorArray get_presets() const;
	PackedColorArray get_recent_presets() const;
	void _update_presets();
	void _update_recent_presets();

	void _select_from_preset_container(const Color &p_color);
	bool _select_from_recent_preset_hbc(const Color &p_color);

	void set_color_mode(ColorModeType p_mode);
	ColorModeType get_color_mode() const;

	void set_colorize_sliders(bool p_colorize_sliders);
	bool is_colorizing_sliders() const;

	void set_deferred_mode(bool p_enabled);
	bool is_deferred_mode() const;

	void set_can_add_swatches(bool p_enabled);
	bool are_swatches_enabled() const;

	void set_presets_visible(bool p_visible);
	bool are_presets_visible() const;

	void set_modes_visible(bool p_visible);
	bool are_modes_visible() const;

	void set_sampler_visible(bool p_visible);
	bool is_sampler_visible() const;

	void set_sliders_visible(bool p_visible);
	bool are_sliders_visible() const;

	void set_hex_visible(bool p_visible);
	bool is_hex_visible() const;

	void set_focus_on_line_edit();
	void set_focus_on_picker_shape();

	ColorPicker();
	~ColorPicker();
};

class ColorPickerPopupPanel : public PopupPanel {
	virtual void _input_from_window(const Ref<InputEvent> &p_event) override;
};

class ColorPickerButton : public Button {
	GDCLASS(ColorPickerButton, Button);

	// Initialization is now done deferred,
	// this improves performance in the inspector as the color picker
	// can be expensive to initialize.

	PopupPanel *popup = nullptr;
	ColorPicker *picker = nullptr;
	Color color;
	bool edit_alpha = true;
	bool edit_intensity = true;

	struct ThemeCache {
		Ref<StyleBox> normal_style;
		Ref<Texture2D> background_icon;

		Ref<Texture2D> overbright_indicator;
	} theme_cache;

	void _about_to_popup();
	void _color_changed(const Color &p_color);
	void _modal_closed();

	virtual void pressed() override;

	void _update_picker();

protected:
	void _notification(int);
	static void _bind_methods();

public:
	void set_pick_color(const Color &p_color);
	Color get_pick_color() const;

	void set_edit_alpha(bool p_show);
	bool is_editing_alpha() const;

	void set_edit_intensity(bool p_show);
	bool is_editing_intensity() const;

	ColorPicker *get_picker();
	PopupPanel *get_popup();

	ColorPickerButton(const String &p_text = String());
};

VARIANT_ENUM_CAST(ColorPicker::PickerShapeType);
VARIANT_ENUM_CAST(ColorPicker::ColorModeType);
