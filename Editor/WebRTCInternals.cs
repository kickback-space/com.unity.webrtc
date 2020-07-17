﻿using System;
using System.Collections;
using System.Collections.Generic;
using Unity.EditorCoroutines.Editor;
using UnityEditor;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.WebRTC.Editor
{
    public delegate void OnPeerListHandler(IEnumerable<RTCPeerConnection> peerList);
    public delegate void OnStatsReportHandler(RTCPeerConnection peer, RTCStatsReport statsReport);

    public class WebRTCInternals : EditorWindow
    {
        [MenuItem("Window/Analysis/WebRTCInternals")]
        public static void Show()
        {
            WebRTCInternals wnd = GetWindow<WebRTCInternals>();
            wnd.titleContent = new GUIContent("WebRTCInternals");
        }

        private const int UpdateStatsInterval = 1;

        public event OnPeerListHandler OnPeerList;
        public event OnStatsReportHandler OnStats;

        private EditorCoroutine m_editorCoroutine;

        private void OnEnable()
        {
            var root = this.rootVisualElement;
            root.Add(CreateStatsView());

            EditorApplication.playModeStateChanged += change =>
            {
                switch (change)
                {
                    case PlayModeStateChange.EnteredPlayMode:
                        m_editorCoroutine = EditorCoroutineUtility.StartCoroutineOwnerless(GetStatsPolling());
                        break;
                    case PlayModeStateChange.ExitingPlayMode:
                        EditorCoroutineUtility.StopCoroutine(m_editorCoroutine);
                        break;
                }
            };

        }

        private void OnDisable()
        {
            EditorCoroutineUtility.StopCoroutine(m_editorCoroutine);
        }

        IEnumerator GetStatsPolling()
        {
            while (true)
            {
                var peerList = WebRTC.PeerList;

                if (peerList != null)
                {
                    OnPeerList?.Invoke(peerList);

                    foreach (var peer in peerList)
                    {
                        var op = peer.GetStats();
                        yield return op;

                        if (!op.IsError)
                        {
                            OnStats?.Invoke(peer, op.Value);
                        }
                    }
                }

                yield return new EditorWaitForSeconds(UpdateStatsInterval);
            }
        }

        private VisualElement CreateStatsView()
        {
            var container = new VisualElement {style = {flexDirection = FlexDirection.Row, flexGrow = 1,}};

            var sideView = new VisualElement
            {
                style = {borderColor = new StyleColor(Color.gray), borderRightWidth = 1, width = 250,}
            };
            var mainView = new VisualElement {style = {flexGrow = 1}};

            container.Add(sideView);
            container.Add(mainView);

            // peer connection list view
            var peerListView = new PeerListView(this);

            sideView.Add(peerListView.Create());

            peerListView.OnChangePeer += newPeer =>
            {
                mainView.Clear();

                // main stats view
                var statsView = new PeerStatsView(newPeer, this);
                mainView.Add(statsView.Create());
            };

            return container;
        }
    }
}
