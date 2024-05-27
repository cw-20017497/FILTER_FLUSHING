Imports System
Imports System.ComponentModel
Imports System.Threading
Imports System.IO.Ports
Imports System.IO
Imports System.Text


Public Class Form
    Dim myPort As Array  '시스템의 검색된 통신포트가 여기에 저장된다
    Dim readString As String
    Dim LogRun As String = "FALSE"

    Delegate Sub UpdateList() '데이터 수신중 쓰레드(thread) 에러 방지용으로 추가함

    Private Sub btnConnect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnConnect.Click

        SerialPort1.PortName = cmbPort.Text         'Set SerialPort1 to the selected COM port at startup
        SerialPort1.BaudRate = cmbBaud.Text         'Set Baud rate to the selected value on
        'Other Serial Port Property
        SerialPort1.Parity = IO.Ports.Parity.None
        SerialPort1.StopBits = IO.Ports.StopBits.One
        SerialPort1.DataBits = 8            'Open our serial port
        SerialPort1.ReadTimeout = 1000


        SerialPort1.Open()

        btnConnect.Enabled = False          'Disable Connect button
        btnDisconnect.Enabled = True        'and Enable Disconnect button

    End Sub

    Private Sub btnDisconnect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDisconnect.Click
        SerialPort1.Close()             'Close our Serial Port
        btnConnect.Enabled = True
        btnDisconnect.Enabled = False
    End Sub
    'Private Sub btnSend_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSend.Click
    '   SerialPort1.Write(txtTransmit.Text & vbCr) 'The text contained in the txtText will be sent to the serial port as ascii
    'plus the carriage return (Enter Key) the carriage return can be ommitted if the other end does not need it
    ' End Sub
    Private Sub SerialPort1_DataReceived(ByVal sender As Object, ByVal e As System.IO.Ports.SerialDataReceivedEventArgs) Handles SerialPort1.DataReceived
        Dim UpdateListBox = New UpdateList(AddressOf UpdateData)

        Me.Invoke(UpdateListBox)
    End Sub

    Private Sub cmbPort_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPort.SelectedIndexChanged
        If SerialPort1.IsOpen = False Then
            SerialPort1.PortName = cmbPort.Text         'pop a message box to user if he is changing ports
        Else                                                                               'without disconnecting first.
            MsgBox(”Valid only if port is Closed”, vbCritical)
        End If
    End Sub
    Private Sub cmbBaud_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbBaud.SelectedIndexChanged
        If SerialPort1.IsOpen = False Then
            SerialPort1.BaudRate = cmbBaud.Text         'pop a message box to user if he is changing baud rate
        Else                                                                                'without disconnecting first.
            MsgBox(”Valid only if port is Closed”, vbCritical)
        End If
    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        '폼 로딩시, 시리얼 포트가 자동 검색되고 cmbPort 콤보박스에 표시된다.
        myPort = IO.Ports.SerialPort.GetPortNames() 'Get all com ports available
        cmbBaud.Items.Add(9600)     'cmbBaud 콤보박스에 사용되는 보드레이트를 표시한다.
        cmbBaud.Items.Add(19200)
        cmbBaud.Items.Add(38400)
        cmbBaud.Items.Add(57600)
        cmbBaud.Items.Add(115200)
        For i = 0 To UBound(myPort)
            cmbPort.Items.Add(myPort(i))
        Next
        cmbPort.Text = cmbPort.Items.Item(0)    'Set cmbPort text to the first COM port detected
        cmbBaud.Text = cmbBaud.Items.Item(0)    'Set cmbBaud text to the first Baud rate on the list
        btnDisconnect.Enabled = False           'Initially Disconnect Button is Disabled

        SerialPort1 = New SerialPort()
    End Sub
    Private Sub UpdateData()

        readString = SerialPort1.ReadLine()

        UpdateDataListBox(readString)
        UpdateDataItem(readString)
    End Sub
    Private Sub UpdateDataListBox(ByVal s As String)
        '리스트 박스에 수신 데이터 표시
        Dim Str As String

        Str = Format(Now(), "MM/dd HH:mm:ss ")
        Str &= s

        listBoxRead.Items.Add(Str)
        listBoxRead.SelectedIndex = listBoxRead.Items.Count - 1

    End Sub
    ' log 문자열
    Dim strType1 As String
    Dim strType2 As String
    Dim strType3 As String
    Dim strType4 As String
    Dim strType5 As String
    Dim strType6 As String
    Dim strType7 As String
    Dim strType8 As String
    Dim strType9 As String
    Dim strType10 As String
    Dim strTypes As String

    Private Sub UpdateDataItem(ByVal s As String)
        Dim str() As String = Split(s, "=")
        Dim Length As Integer

        If (str.Count >= 2) Then
            If (str(0) = "TYPE_1") Then
                UpdateDataItemType_1(str(1))
                Length = str(1).Length
                str(1) = str(1).Remove(Length - 2, 2)
                strType1 = str(1)
            End If
        End If
    End Sub

    Private Sub UpdateDataItemType_1(ByVal s As String)
        Dim str() As String = Split(s, "@")

        If (str.Count >= 3) Then
            Dim strValve() As String = Split(str(0), ":")
            Dim strSetting() As String = Split(str(1), ":")

            DispValve(strValve) '밸브 상태
            DispSetting(strSetting) '설정 정보

        End If
    End Sub



    Private Sub DispValve(ByVal s() As String)
        DispOpen(s(0), lbValveFeed)
        DispOpen(s(1), lbValveFlushing)
        DispOpen(s(2), lbValveOut)

        DispOnOff(s(3), lbPumpOnOff)
        lbPumpFeedback.Text = "(" & s(4) & " ADC )"

    End Sub

    Private Sub DispSetting(ByVal s() As String)

        lbSetting1.Text = s(0) & " 초"
        lbSetting2.Text = s(1) & " 초"
        lbSetting3.Text = s(2) & " 초"
        lbSetting4.Text = s(3) & " 초"
        lbSetting5.Text = s(4) & " 초"
        lbSetting6.Text = s(5) & " 초"

        lbWaitTime.Text = s(6) & " 초"

        DispReverseStep(s(7))

        lbRepeat.Text = s(8) & " 번"
        lbRepeat3.Text = "( " & s(9) & " )"

    End Sub
    Sub DispReverseStep(ByVal s As String)

        If (s = 0) Then
            lbStep1.ForeColor = Color.Blue
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 1) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Blue
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 2) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Blue
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 3) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Blue
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 4) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Blue
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 5) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Blue
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 6) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Blue
            lbStep8.ForeColor = Color.Black
        ElseIf (s = 7) Then
            lbStep1.ForeColor = Color.Black
            lbStep2.ForeColor = Color.Black
            lbStep3.ForeColor = Color.Black
            lbStep4.ForeColor = Color.Black
            lbStep5.ForeColor = Color.Black
            lbStep6.ForeColor = Color.Black
            lbStep7.ForeColor = Color.Black
            lbStep8.ForeColor = Color.Blue
        End If

    End Sub
    Sub DispOpen(ByVal s As String, ByRef lb As Label)
        If s = "0" Then
            lb.Text = "닫힘"
            lb.ForeColor = Color.Red
        Else
            lb.Text = "열림"
            lb.ForeColor = Color.Blue
        End If
    End Sub
    Sub DispOnOff(ByVal s As String, ByRef lb As Label)
        If s = "0" Then
            lb.Text = "OFF"
            lb.ForeColor = Color.Red
        Else
            lb.Text = "On"
            lb.ForeColor = Color.Blue
        End If
    End Sub

    Sub DispError(ByVal s As String, ByRef lb As Label)
        If s = "0" Then
            lb.Text = "정상"
            lb.ForeColor = Color.Black
        Else
            lb.Text = "에러"
            lb.ForeColor = Color.Red
        End If
    End Sub


    ''
    '' 버튼 제어
    ''

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick

        'FormHealth.lbCurrentDate.Text = Format(Now(), "yyyy/MM/dd HH:mm:ss")
        'If (LogRun = "TRUE") Then
        '    btnSaveEnable.Enabled = False
        '    btnSaveDisable.Enabled = True
        'Else
        '    btnSaveEnable.Enabled = True
        '    btnSaveDisable.Enabled = False
        'End If
    End Sub

    ' 로그 파일 저장
    Dim LogFileName As String ' 파일 이름
    Dim fs As FileStream ' 파일 stream

    Dim info As Byte()

    Private Sub OpenLogFile()
        Dim path As String

        Dim strTitle As String
        Dim logByte() As Byte


        strTitle = "시간 "
        strTitle = strTitle & "냉수_만수위	냉수_수위	냉수_최초만수위	V_NOS	V_정수추출	V_냉수에어	V_냉수입수	V_냉수추출	V_냉수배수	V_온수입수	V_온수출수	V_온수배수	V_전원	V_트레이_입수	압축기	FEEDER_CW	FEEDER_CCW	ICE_TRAY_CW	ICE_TRAY_CCW	HZ	LPM	Outage	얼음_Config	얼음_Make	얼음_우선	제빙_모드	만빙_상태	만빙_시작	만빙_출력	만빙_입력	만빙_카운터	만빙_검사시간	만빙_검사주기	Init_Step	Ready_Step	Icing_Step	정수온도	외기온도	트레이입수시간	제빙시간	스윙바	DeIcing_Step	탈빙_온도_영역	탈빙_RepeatNum	탈빙_Count	탈빙_OnOffCmd	탈빙_OnOffTime	Tray_탈빙위치	Tray_제빙위치	Tray_StartTime	Tray_MovingTime	Tray_RR_Count	Tray_RR_Wait	Tray_RCF_Cmd	Tray_RCF_CmdCount	Tray_RCF_TIME	Tray_RFD_WaitTime	Tray_ErrorIcingCount	Tray_ErrorDeIcingCount	Cold_Config	Cold_Make	Cold_TargetTempOn	Cold_TargetTempOff	Cold_현재온도	Cold_지연입수시간	Cold_추가입수시간	Cold_추가냉각	Cold_추가냉각시간	Comp_OnOff	Comp_보호시간	Comp_OnTime	Comp_OffTime	Comp_FANOnOff_1	Comp_FANOnOff_2	Comp_추가동작시간	Comp_목표RPS	Comp_현재RPS	Comp_에러코드	DrainPump_OnOff	RetryCount	RetryCmd	OnOffTime	Error	저수위	만수위	수위상태	상태	모드	목표_위치	현재_위치	ADC_TEMP_HOT_IN	TEMP_HOT_OUT	TEMP_HEATER	TEMP_ROOM	TEMP_COLD	TEMP_AMBI	TEMP_EVA_1	LEAK	SAVING_PHOTO	만빙	배수펌프_FB	UV_ICE_TANK	UV_ICE_TRAY	UV_WATER_OUT	UV_ICE_DOOR	HEATER_FB	24V_FB	12V_FB	TEMP_HOT_IN	TEMP_HOT_OUT	TEMP_HEATER	TEMP_ROOM	TEMP_COLD	TEMP_AMBI	TEMP_EVA_1	년	월	일	시	분	초	RTC레지스터	Saving_Config	상태	Dark2Light	Light2Dark	IsDark	ADC_Photo	AdcLimitSleep	AdcLimitWakeUp	TimerSleep	TimerWakeUp	Err_ColdTemp	Err_RoomTemp	Err_AmbiTemp	Err_HotInTemp	Err_HotOut	Err_HeaterTemp	Err_Eva	Err_Leak	Err_MicroDetect	Err_MircoMove	Err_DrainPump	서비스_상태	탱크열림	시간	Eep저장시간	온수_Lock	초기화	TargetOutTemp	TargetPreHeatTemp	CoolingTime	MaxTimeOut	PreHeatingTimeOut	Output	MaxTick	CurrentTick	TargetTick	히터_상_OnOff	히터_하_OnOff	상태	오프셋	밸브 목표 위치	밸브 현재 위치	UV_ICE_TANK	UV_ICE_TRAY	UV_ICE_DOOR	UV_WATER_OUT	냉수_온도_읽기_시간	정수_온도_읽기_시간	클래스	BaseWatt	BaseOffsetWatt	Watt	TargetWatt	TargetOffsetWatt"
        strTitle = Replace(strTitle, Chr(13), "")

        LogFileName = "\AIS_SODA_LOG_" & Format(Now(), "yyMMdd_HHmmss") & ".csv"
        path = System.IO.Directory.GetCurrentDirectory() & LogFileName

        ' Create or overwrite the file.
        fs = File.Create(path)

        logByte = Encoding.Default.GetBytes(strTitle & Environment.NewLine)
        fs.Write(logByte, 0, logByte.Length)
    End Sub

    ' 파일 쓰기
    Dim LogCount As Integer
    Private Sub SaveLogBytes(b() As Byte)
        '   Dim str As String
        If (LogRun = "TRUE") Then
            fs.Write(b, 0, b.Length)

            LogCount = LogCount + 1
            If (LogCount >= 8000) Then
                LogCount = 0
                fs.Close()
                OpenLogFile()

            End If
        End If
    End Sub



    Private Sub SendMessage(ByVal upper As String, ByVal lower As String)
        Dim buf As Byte() = New Byte(100) {}
        Dim buf_2 As Byte() = New Byte(100) {}
        Dim str_1 As String
        Dim Chksum As Byte

        Dim v As Byte

        buf(0) = Hex(1) ' STX(0x01)

        ' MESSAGE TYPE 0x000 ~ 0xFFFF
        buf(1) = 48 '0'
        buf(2) = 48 '0'

        buf(3) = upper
        buf(4) = lower

        If buf(3) > 9 Then
            buf(3) = buf(3) + 55
        Else
            buf(3) = buf(3) + 48
        End If

        If buf(4) > 9 Then
            buf(4) = buf(4) + 55
        Else
            buf(4) = buf(4) + 48
        End If

        buf(5) = 0
        buf(6) = 0

        ' Chksum Calc
        Chksum = buf(0)
        Chksum = Chksum Xor buf(1)
        Chksum = Chksum Xor buf(2)
        Chksum = Chksum Xor buf(3)
        Chksum = Chksum Xor buf(4)
        Chksum = Chksum Xor buf(5)
        Chksum = Chksum Xor buf(6)

        ' Chksum
        v = Fix(Chksum / 16)
        If v > 9 Then
            buf(7) = v + 55
        Else
            buf(7) = v + 48
        End If


        v = Chksum Mod 16
        If v > 9 Then
            buf(8) = v + 55
        Else
            buf(8) = v + 48
        End If


        ' ETX(0x04)
        buf(9) = Hex(4)


        ' Make String...
        buf_2(0) = buf(3)
        buf_2(1) = buf(4)
        buf_2(2) = buf(5)
        buf_2(3) = buf(6)
        buf_2(4) = buf(7)
        buf_2(5) = buf(8)

        str_1 = System.Text.Encoding.UTF8.GetString(buf_2)
        UpdateDataListBox(str_1)

        SerialPort1.Write(buf, 0, 10)

    End Sub

    Sub SendMessageControl(ByVal upper As String, ByVal lower As String, ByVal msg_upper As String, ByVal msg_lower As String)
        Dim buf As Byte() = New Byte(100) {}
        Dim buf_2 As Byte() = New Byte(100) {}
        Dim str_1 As String
        Dim Chksum As Byte

        Dim v As Byte

        buf(0) = Hex(1) ' STX(0x01)

        ' MESSAGE TYPE 0x000 ~ 0xFFFF
        buf(1) = 48 '0'
        buf(2) = 48 '0'

        buf(3) = upper
        buf(4) = lower

        If buf(3) > 9 Then
            buf(3) = buf(3) + 55
        Else
            buf(3) = buf(3) + 48
        End If

        If buf(4) > 9 Then
            buf(4) = buf(4) + 55
        Else
            buf(4) = buf(4) + 48
        End If


        buf(5) = msg_upper
        buf(6) = msg_lower
        If buf(5) > 9 Then
            buf(5) = buf(5) + 55
        Else
            buf(5) = buf(5) + 48
        End If

        If buf(6) > 9 Then
            buf(6) = buf(6) + 55
        Else
            buf(6) = buf(6) + 48
        End If


        ' Chksum Calc
        Chksum = buf(0)
        Chksum = Chksum Xor buf(1)
        Chksum = Chksum Xor buf(2)
        Chksum = Chksum Xor buf(3)
        Chksum = Chksum Xor buf(4)
        Chksum = Chksum Xor buf(5)
        Chksum = Chksum Xor buf(6)

        ' Chksum
        v = Fix(Chksum / 16)
        If v > 9 Then
            buf(7) = v + 55
        Else
            buf(7) = v + 48
        End If


        v = Chksum Mod 16
        If v > 9 Then
            buf(8) = v + 55
        Else
            buf(8) = v + 48
        End If


        ' ETX(0x04)
        buf(9) = Hex(4)


        ' Make String...
        buf_2(0) = buf(3)
        buf_2(1) = buf(4)
        buf_2(2) = buf(5)
        buf_2(3) = buf(6)
        buf_2(4) = buf(7)
        buf_2(5) = buf(8)

        str_1 = System.Text.Encoding.UTF8.GetString(buf_2)
        UpdateDataListBox(str_1)

        If (SerialPort1.BytesToWrite() = 0) Then

            SerialPort1.Write(buf, 0, 10)
        End If

    End Sub





    '100ms 주기로 데이터 요청 메시지 전달

    Dim MsgCtrId As String

    Sub SetMessageId(msg As String)
        MsgCtrId = msg
    End Sub

    Private Function CheckCtrId()

        If MsgCtrId IsNot "NONE" Then

            If MsgCtrId = "RELEASE_AIR_UP" Then
                SendMessageControl(14, 0, "1", 0)

            ElseIf MsgCtrId = "RELEASE_AIR_DOWN" Then
                SendMessageControl(14, 0, "2", 0)

            ElseIf MsgCtrId = "IN_AIR_UP" Then
                SendMessageControl(14, 0, "3", 0)

            ElseIf MsgCtrId = "IN_AIR_DOWN" Then
                SendMessageControl(14, 0, "4", 0)

            ElseIf MsgCtrId = "PRESSURE_AIR_UP" Then
                SendMessageControl(14, 0, "5", 0)

            ElseIf MsgCtrId = "PRESSURE_AIR_DOWN" Then
                SendMessageControl(14, 0, "6", 0)

            ElseIf MsgCtrId = "BREAK_UP" Then
                SendMessageControl(14, 0, "7", 0)

            ElseIf MsgCtrId = "BREAK_DOWN" Then
                SendMessageControl(14, 0, "8", 0)

            ElseIf MsgCtrId = "FLUSHING_UP" Then
                SendMessageControl(14, 0, "9", 0)

            ElseIf MsgCtrId = "FLUSHING_DOWN" Then
                SendMessageControl(14, 0, 10, 0)

            ElseIf MsgCtrId = "OUT_UP" Then
                SendMessageControl(14, 0, 11, 0)

            ElseIf MsgCtrId = "OUT_DOWN" Then
                SendMessageControl(14, 0, 12, 0)

            ElseIf MsgCtrId = "SAVE" Then
                SendMessageControl(14, 0, 28, 0)

            ElseIf MsgCtrId = "SKIP" Then
                SendMessageControl(14, 0, 13, 0)

            ElseIf MsgCtrId = "RESET" Then
                SendMessageControl(14, 0, 14, 0)

            ElseIf MsgCtrId = "RTC_CURRENT" Then
                Dim str As String
                Dim dt As Date = Date.Now

                str = "C" & Format(Now(), "yyyy/MM/dd HH:mm:ss")
                SendMessageControl(14, 0, str, 0)
            End If

            MsgCtrId = "NONE"
            CheckCtrId = True
        Else
            CheckCtrId = False
        End If

    End Function


    Dim MsgId As Integer
    Private Sub Timer2_Tick(sender As Object, e As EventArgs) Handles Timer2.Tick


        If SerialPort1.IsOpen = True Then

            If CheckCtrId() = True Then
                Exit Sub
            End If

            '  MsgId = MsgId + 1
            SendMessage(14, 1)

            'If MsgId = 1 Then
            '            MsgId = 1
            'End If
        End If

    End Sub

    Private Sub btnUp1_Click(sender As Object, e As EventArgs) Handles btnUp1.Click
        SetMessageId("RELEASE_AIR_UP")
    End Sub

    Private Sub btnDown1_Click(sender As Object, e As EventArgs) Handles btnDown1.Click
        SetMessageId("RELEASE_AIR_DOWN")
    End Sub

    Private Sub btnUp2_Click(sender As Object, e As EventArgs) Handles btnUp2.Click
        SetMessageId("IN_AIR_UP")
    End Sub

    Private Sub btnDown2_Click(sender As Object, e As EventArgs) Handles btnDown2.Click
        SetMessageId("IN_AIR_DOWN")
    End Sub

    Private Sub btnUp3_Click(sender As Object, e As EventArgs) Handles btnUp3.Click
        SetMessageId("PRESSURE_AIR_UP")
    End Sub

    Private Sub btnDown3_Click(sender As Object, e As EventArgs) Handles btnDown3.Click
        SetMessageId("PRESSURE_AIR_DOWN")
    End Sub

    Private Sub btnUp4_Click(sender As Object, e As EventArgs) Handles btnUp4.Click
        SetMessageId("BREAK_UP")
    End Sub

    Private Sub btnDown4_Click(sender As Object, e As EventArgs) Handles btnDown4.Click
        SetMessageId("BREAK_DOWN")
    End Sub

    Private Sub btnUp5_Click(sender As Object, e As EventArgs) Handles btnUp5.Click
        SetMessageId("FLUSHING_UP")
    End Sub

    Private Sub btnDown5_Click(sender As Object, e As EventArgs) Handles btnDown5.Click
        SetMessageId("FLUSHING_DOWN")
    End Sub

    Private Sub btnUp6_Click(sender As Object, e As EventArgs) Handles btnUp6.Click
        SetMessageId("OUT_UP")
    End Sub

    Private Sub btnDown6_Click(sender As Object, e As EventArgs) Handles btnDown6.Click
        SetMessageId("OUT_DOWN")
    End Sub

    Private Sub btnSaveSetting_Click(sender As Object, e As EventArgs) Handles btnSaveSetting.Click
        SetMessageId("SAVE")
    End Sub

    Private Sub btnSkip_Click(sender As Object, e As EventArgs) Handles btnSkip.Click
        SetMessageId("SKIP")
    End Sub

    Private Sub btnReset_Click(sender As Object, e As EventArgs) Handles btnReset.Click
        SetMessageId("RESET")
    End Sub
End Class

