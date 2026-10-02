/*
 * audioview.cpp - FSSAudioView implementation
 *
 * Copyright (C) 2016  Wicked_Digger <wicked_digger@mail.ru>
 *
 * This file is part of FSStudio.
 *
 * FSStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FSStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FSStudio.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/audioview.h"

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTemporaryFile>
#include <QDir>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>
#include <QUrl>
#include <QDebug>

#include "src/buffer.h"

FSSAudioView::FSSAudioView(QWidget *pParent) : QWidget(pParent) {
  file = nullptr;
  player = new QMediaPlayer(this);
  audioOutput = new QAudioOutput(this);
  audioOutput->setVolume(0.5f);
  player->setAudioOutput(audioOutput);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  setLayout(main_layout);

  buttonPlay = new QPushButton(this);
  buttonPlay->setText("Play");
  buttonPlay->setEnabled(false);
  main_layout->addWidget(buttonPlay);
  connect(buttonPlay, &QPushButton::clicked,
          this, &FSSAudioView::on_play);

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  slider->setEnabled(false);
  main_layout->addWidget(slider);
  connect(slider, &QSlider::sliderMoved,
          player, &QMediaPlayer::setPosition);

  connect(player, &QMediaPlayer::mediaStatusChanged,
          this, &FSSAudioView::on_media_status_changed);
  connect(player, &QMediaPlayer::durationChanged,
          this, &FSSAudioView::on_duration_changed);
  connect(player, &QMediaPlayer::positionChanged,
          this, &FSSAudioView::on_position_changed);
  connect(player, &QMediaPlayer::playbackStateChanged,
          this, &FSSAudioView::on_state_changed);
  connect(player, &QMediaPlayer::errorOccurred,
          this, &FSSAudioView::on_error);
}

FSSAudioView::~FSSAudioView() {
  player->disconnect();
  player->stop();
  player->setSource(QUrl());

  if (file != nullptr) {
    delete file;
    file = nullptr;
  }
}

void
FSSAudioView::setAudioData(PBuffer data, const QString &format) {
  player->stop();
  player->setSource(QUrl());
  if (file != nullptr) {
    delete file;
    file = nullptr;
  }
  buttonPlay->setText("Play");
  buttonPlay->setEnabled(false);
  slider->setEnabled(false);
  slider->setValue(0);

  if (!data) {
    return;
  }

  file = new QTemporaryFile(QDir::tempPath() + "/XXXXXX." + format);
  if (!file->open()) {
    qDebug() << "Failed to open temporary file";
    return;
  }
  qint64 size = data->get_size();
  if (file->write((const char*)data->get_data(), size) != size) {
    qDebug() << "Failed to write to temporary file";
    return;
  }
  file->close();

  player->setSource(QUrl::fromLocalFile(file->fileName()));
}

void
FSSAudioView::on_media_status_changed(QMediaPlayer::MediaStatus status) {
  switch (status) {
    case QMediaPlayer::LoadedMedia:
      buttonPlay->setEnabled(true);
      slider->setEnabled(true);
      break;
    case QMediaPlayer::EndOfMedia:
      player->setPosition(0);
      break;
    case QMediaPlayer::InvalidMedia:
      buttonPlay->setEnabled(false);
      slider->setEnabled(false);
      break;
    default:
      break;
  }
}

void
FSSAudioView::on_duration_changed(qint64 duration) {
  slider->setMinimum(0);
  slider->setMaximum((int)duration);
}

void
FSSAudioView::on_position_changed(qint64 position) {
  if (!slider->isSliderDown()) {
    slider->setValue((int)position);
  }
}

void
FSSAudioView::on_state_changed(QMediaPlayer::PlaybackState state) {
  switch (state) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
      buttonPlay->setText("Play");
      break;
    case QMediaPlayer::PlayingState:
      buttonPlay->setText("Stop");
      break;
  }
}

void
FSSAudioView::on_error(QMediaPlayer::Error /*error*/,
                       const QString &errorString) {
  qDebug() << "Audio playback error:" << errorString;
}

void
FSSAudioView::on_play() {
  if (player->playbackState() == QMediaPlayer::PlayingState) {
    player->stop();
  } else {
    player->play();
  }
}
