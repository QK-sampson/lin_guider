/*
 * lin_guider.h
 *
 *      Author: gm
 *
 *
 * This file is part of Lin_guider.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIN_GUIDER_H
#define LIN_GUIDER_H

#include <QtGui>
#include <assert.h>
#include "ui_lin_guider.h"

#include "common.h"
#include "setup_driver.h"
#include "setup_video.h"
#include "rcalibration.h"
#include "mrecorder.h"
#include "server.h"
#include "guider.h"
#include "gmath.h"
#include "params.h"
#include "about.h"
#include "server.h"
#include "settings.h"


typedef struct
{
	bool active;
	int type;
}drag_object_t;


class drawer_delegate;


class lin_guider : public QMainWindow
{
    Q_OBJECT

friend class setup_video;
friend class setup_driver;
friend class guider;
friend class rcalibration;
friend class mrecorder;
//friend class server;
friend class drawer_delegate;

public:
    lin_guider(QWidget *parent = 0);
    ~lin_guider();


    void lock_toolbar( bool lock );
    void set_visible_overlays( int ovr_mask, bool set );
    // test stuff

protected slots:
	// tool bar
	void onShowSetupGuider();
	void onShowSetupDriver();
	void onRecord();
	void onShowCalibration();
	void onShowGuiding();
	void onShowSettings();
	void onShowAbout();
	void onActionExit();
	//
	void onGetVideo(const void *, int);
	void onRemoteCmd( void );

	void onCmdTimer();
protected:
	void showEvent ( QShowEvent * event );
	void closeEvent( QCloseEvent *event );

private:
	params *param_block;

	video_drv::cvideo_base *m_video;
	io_drv::cio_driver_base *m_driver;
	server *m_server;
	setup_video *setup_video_wnd;
	setup_driver *setup_driver_wnd;
	guider *guider_wnd;
	cgmath *m_math;
	rcalibration *reticle_wnd;
	mrecorder *recorder_wnd;
	settings *settings_wnd;
	about *about_wnd;

	custom_drawer *m_video_out; 	// Drawing widget
	u_char *m_v_buf;
	QImage  *m_video_buffer;
	QColor SQR_OVL_COLOR, RA_COLOR, DEC_COLOR, RET_ORG_COLOR;

	video_drv::captureparams_t capture_params;
	guiderparams_t  guider_params;
	uiparams_t ui_params;
	io_drv::device_init_params_t device_params;
	calibrationparams_t calibration_params;
	net_params_t m_net_params;
	common_params m_common_params;
	guider::drift_view_params_s m_drift_view_params;

	// dev names
	char dev_name_video[64];
	char dev_name_io[64];

	bool activate_drag_object( int x, int y );
	bool deactivate_drag_object( int x, int y );
	void move_drag_object( int x, int y );

	void update_sb_video_info( int override_fps_idx = -1 );
	void update_sb_io_info( void );

	drag_object_t d_objs[2];
	drawer_delegate *m_mouse_delegate;

	conn_t *m_long_task_conn;

private:
    Ui::lin_guiderClass ui;
    QLabel *m_video_name_label;
    QLabel *m_io_name_label;

    QTimer m_timer;
};




class drawer_delegate : public mouse_delegate
{
public:
	explicit drawer_delegate( lin_guider *parent ) : m_parent(parent), m_dragging(false)
	{
		assert(parent);
	}
	void mouse_press( QMouseEvent *event )
	{
		if( event->button() != Qt::LeftButton || !m_parent->activate_drag_object( event->x(), event->y() ) )
    		return;
		m_dragging = true;
	}
	void mouse_release( QMouseEvent *event )
	{
		m_parent->deactivate_drag_object( event->x(), event->y() );
		m_dragging = false;
	}
	void mouse_move( QMouseEvent *event )
	{
		if( !m_dragging )
    		return;

		m_parent->move_drag_object( event->x(), event->y() );
	}
private:
	lin_guider *m_parent;
	bool m_dragging;

};

#endif // LIN_GUIDER_H
