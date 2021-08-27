#ifndef GUI_H // include guard
#define GUI_H

#include <gtkmm.h>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
using std::cout, std::endl, std::string;
using cv::Mat;

class MyArea : public Gtk::DrawingArea
{
public:
  MyArea();
  virtual ~MyArea();

  void change_image(string filepath);
  void change_image(Glib::RefPtr<Gdk::Pixbuf> image);
  void change_image(Mat image);

protected:
  //Override default signal handler:
  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

  Glib::RefPtr<Gdk::Pixbuf> m_image;
};


class App : public Gtk::Application
{

public:
  App();
  ~App();

  Gtk::Window* window;

protected:
  //Member widgets:
  Glib::RefPtr<Gtk::Builder> builder;

  Gtk::Viewport* viewport;
  Gtk::Grid* grid;

  Gtk::FileChooserDialog* imgview_file_dialog;
  Gtk::FileChooserDialog* imgview_file_dialog2;

  Gtk::Entry* entry_feature;
  Gtk::Entry* entry_match;
  Gtk::Entry* Threshold;

  Gtk::Button* b_open;
  Gtk::Button* b_open2;
  Gtk::Button* b_feature;
  Gtk::Button* b_match;

  Gtk::Label* comp_time;

  Gtk::EventBox* imbox1;
  Gtk::EventBox* imbox2;
  MyArea imgwin1;
  MyArea imgwin2;

  Mat img_mat1;
  Mat img_mat2;

protected:
  // Signal handlers:
  void imgview_chose_file (int response_id);
  void imgview_chose_file2 (int response_id);

  void btn_open();
  void btn_open2();
  void btn_feature();
  void btn_match();

  bool imgbox(GdkEventButton* event);

protected:
  // Member initializer functions
  void get_widgets();
  void connect_signals();
};

#endif
