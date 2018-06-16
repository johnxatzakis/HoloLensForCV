//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "SensorStreamViewer.g.h"
#include "MainPage.xaml.h"
#include "SimpleLogger.h"
#include "FrameRenderer.h"
#include "FrameSourceViewModels.h"

namespace SensorStreaming
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SensorStreamViewer sealed
    {
    public:
        SensorStreamViewer();
        virtual ~SensorStreamViewer();

        void Start();

        void Stop();

    protected: // Protected UI overrides.
        /// <summary>
        /// Called when user navigates to this Scenario.
        /// Immediately start streaming from first available source group.
        /// </summary>
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        /// <summary>
        /// Called when user navigates away from this Scenario.
        /// Stops streaming and disposes of all objects.
        /// </summary>
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        // Private UI methods.
        /// <summary>
        /// Select next available source group and start streaming from it.
        /// </summary>
        void NextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PlayStopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        // Private methods.
        /// <summary>
        /// Disable the Next button while we switch to the next eligible media source.
        /// </summary>
        concurrency::task<void> LoadMediaSourceAsync();

        /// <summary>
        /// Switch to the next eligible media source.
        /// </summary>
        concurrency::task<void> LoadMediaSourceWorkerAsync();

        /// <summary>
        /// Stop streaming from all readers and dispose all readers and media capture object.
        /// </summary>
        concurrency::task<void> CleanupMediaCaptureAsync();

        /// <summary>
        /// Initialize the media capture object.
        /// Must be called from the UI thread.
        /// </summary>
        concurrency::task<bool> TryInitializeMediaCaptureAsync(
            Windows::Media::Capture::Frames::MediaFrameSourceGroup^ group);

        /// <summary>
        /// Handler for frames which arrive from the MediaFrameReader.
        /// </summary>
        void FrameReader_FrameArrived(
            Windows::Media::Capture::Frames::MediaFrameReader^ sender,
            Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

        static bool GetSensorName(
            Windows::Media::Capture::Frames::MediaFrameSource^ source, Platform::String^& name);

        static void DebugOutputAllProperties(Windows::Foundation::Collections::IMapView<Platform::Guid, Platform::Object^>^ properties);

        void OnPointerPressed(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);

    private:
        // Private data.
        MainPage^ rootPage = MainPage::Current;

        SimpleLogger^ m_logger;

        Platform::Agile<Windows::Media::Capture::MediaCapture> m_mediaCapture;
        int m_selectedSourceGroupIndex{ 1 };

        struct VolatileState
        {
            std::mutex m_mutex;

            // The currently selected source group.
            int m_selectedStreamId{ 1 };

            std::vector<std::pair<Windows::Media::Capture::Frames::MediaFrameReader^, Windows::Foundation::EventRegistrationToken>> m_readers;

            //std::map<Windows::Media::Capture::Frames::MediaFrameSourceKind, FrameRenderer^> m_frameRenderers;
            std::map<int, FrameRenderer^> m_frameRenderers;
            std::map<int, int> m_FrameReadersToSourceIdMap;
            std::map<int, int> m_frameCount;

            // Setting this to false enabled displaying all the streams atlease for one frame
            bool m_firstRunComplete{ false };
        } m_volatileState;
    };
}