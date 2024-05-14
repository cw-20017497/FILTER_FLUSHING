<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Form
    Inherits System.Windows.Forms.Form

    'Form은 Dispose를 재정의하여 구성 요소 목록을 정리합니다.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows Form 디자이너에 필요합니다.
    Private components As System.ComponentModel.IContainer

    '참고: 다음 프로시저는 Windows Form 디자이너에 필요합니다.
    '수정하려면 Windows Form 디자이너를 사용하십시오.  
    '코드 편집기에서는 수정하지 마세요.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.lbValveOut = New System.Windows.Forms.Label()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.lbValveFlushing = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.lbPumpFeedback = New System.Windows.Forms.Label()
        Me.btnSkip = New System.Windows.Forms.Button()
        Me.lbPumpOnOff = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.lbValveFeed = New System.Windows.Forms.Label()
        Me.Label26 = New System.Windows.Forms.Label()
        Me.lbWaitTime = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.btnConnect = New System.Windows.Forms.Button()
        Me.btnDisconnect = New System.Windows.Forms.Button()
        Me.cmbPort = New System.Windows.Forms.ComboBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.cmbBaud = New System.Windows.Forms.ComboBox()
        Me.SerialPort1 = New System.IO.Ports.SerialPort(Me.components)
        Me.listBoxRead = New System.Windows.Forms.ListBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox9 = New System.Windows.Forms.GroupBox()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.BackgroundWorker1 = New System.ComponentModel.BackgroundWorker()
        Me.Timer2 = New System.Windows.Forms.Timer(Me.components)
        Me.GroupBox14 = New System.Windows.Forms.GroupBox()
        Me.Label106 = New System.Windows.Forms.Label()
        Me.btnDown6 = New System.Windows.Forms.Button()
        Me.btnDown3 = New System.Windows.Forms.Button()
        Me.btnDown5 = New System.Windows.Forms.Button()
        Me.btnDown2 = New System.Windows.Forms.Button()
        Me.btnSaveSetting = New System.Windows.Forms.Button()
        Me.btnDown4 = New System.Windows.Forms.Button()
        Me.btnDown1 = New System.Windows.Forms.Button()
        Me.btnUp6 = New System.Windows.Forms.Button()
        Me.btnUp3 = New System.Windows.Forms.Button()
        Me.btnUp5 = New System.Windows.Forms.Button()
        Me.btnUp2 = New System.Windows.Forms.Button()
        Me.btnUp4 = New System.Windows.Forms.Button()
        Me.btnUp1 = New System.Windows.Forms.Button()
        Me.lbSetting3 = New System.Windows.Forms.Label()
        Me.lbSetting2 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lbSetting4 = New System.Windows.Forms.Label()
        Me.Label121 = New System.Windows.Forms.Label()
        Me.lbSetting6 = New System.Windows.Forms.Label()
        Me.lbSetting1 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.lbSetting5 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.PictureBox1 = New System.Windows.Forms.PictureBox()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.lbStep8 = New System.Windows.Forms.Label()
        Me.lbStep7 = New System.Windows.Forms.Label()
        Me.lbStep6 = New System.Windows.Forms.Label()
        Me.lbStep5 = New System.Windows.Forms.Label()
        Me.lbStep4 = New System.Windows.Forms.Label()
        Me.lbStep3 = New System.Windows.Forms.Label()
        Me.lbStep2 = New System.Windows.Forms.Label()
        Me.lbStep1 = New System.Windows.Forms.Label()
        Me.lbRepeat = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.btnReset = New System.Windows.Forms.Button()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox9.SuspendLayout()
        Me.GroupBox14.SuspendLayout()
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.lbValveOut)
        Me.GroupBox3.Controls.Add(Me.Label22)
        Me.GroupBox3.Controls.Add(Me.lbValveFlushing)
        Me.GroupBox3.Controls.Add(Me.Label16)
        Me.GroupBox3.Controls.Add(Me.lbPumpFeedback)
        Me.GroupBox3.Controls.Add(Me.btnSkip)
        Me.GroupBox3.Controls.Add(Me.lbPumpOnOff)
        Me.GroupBox3.Controls.Add(Me.Label1)
        Me.GroupBox3.Controls.Add(Me.lbValveFeed)
        Me.GroupBox3.Controls.Add(Me.Label26)
        Me.GroupBox3.Controls.Add(Me.lbWaitTime)
        Me.GroupBox3.Controls.Add(Me.Label5)
        Me.GroupBox3.Location = New System.Drawing.Point(12, 285)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(202, 207)
        Me.GroupBox3.TabIndex = 12
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "부하"
        '
        'lbValveOut
        '
        Me.lbValveOut.AutoSize = True
        Me.lbValveOut.Location = New System.Drawing.Point(79, 77)
        Me.lbValveOut.Name = "lbValveOut"
        Me.lbValveOut.Size = New System.Drawing.Size(29, 12)
        Me.lbValveOut.TabIndex = 18
        Me.lbValveOut.Text = "닫힘"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(41, 77)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(33, 12)
        Me.Label22.TabIndex = 12
        Me.Label22.Text = "출수:"
        '
        'lbValveFlushing
        '
        Me.lbValveFlushing.AutoSize = True
        Me.lbValveFlushing.Location = New System.Drawing.Point(79, 53)
        Me.lbValveFlushing.Name = "lbValveFlushing"
        Me.lbValveFlushing.Size = New System.Drawing.Size(29, 12)
        Me.lbValveFlushing.TabIndex = 9
        Me.lbValveFlushing.Text = "닫힘"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(41, 26)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(33, 12)
        Me.Label16.TabIndex = 0
        Me.Label16.Text = "입수:"
        '
        'lbPumpFeedback
        '
        Me.lbPumpFeedback.AutoSize = True
        Me.lbPumpFeedback.Location = New System.Drawing.Point(114, 106)
        Me.lbPumpFeedback.Name = "lbPumpFeedback"
        Me.lbPumpFeedback.Size = New System.Drawing.Size(40, 12)
        Me.lbPumpFeedback.TabIndex = 8
        Me.lbPumpFeedback.Text = "(ADC)"
        '
        'btnSkip
        '
        Me.btnSkip.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnSkip.Location = New System.Drawing.Point(15, 154)
        Me.btnSkip.Name = "btnSkip"
        Me.btnSkip.Size = New System.Drawing.Size(177, 47)
        Me.btnSkip.TabIndex = 54
        Me.btnSkip.Text = "다음 단계"
        Me.btnSkip.UseVisualStyleBackColor = True
        '
        'lbPumpOnOff
        '
        Me.lbPumpOnOff.AutoSize = True
        Me.lbPumpOnOff.Location = New System.Drawing.Point(80, 106)
        Me.lbPumpOnOff.Name = "lbPumpOnOff"
        Me.lbPumpOnOff.Size = New System.Drawing.Size(28, 12)
        Me.lbPumpOnOff.TabIndex = 8
        Me.lbPumpOnOff.Text = "OFF"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(41, 106)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(33, 12)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "펌프:"
        '
        'lbValveFeed
        '
        Me.lbValveFeed.AutoSize = True
        Me.lbValveFeed.Location = New System.Drawing.Point(79, 26)
        Me.lbValveFeed.Name = "lbValveFeed"
        Me.lbValveFeed.Size = New System.Drawing.Size(29, 12)
        Me.lbValveFeed.TabIndex = 5
        Me.lbValveFeed.Text = "닫힘"
        '
        'Label26
        '
        Me.Label26.AutoSize = True
        Me.Label26.Location = New System.Drawing.Point(32, 53)
        Me.Label26.Name = "Label26"
        Me.Label26.Size = New System.Drawing.Size(45, 12)
        Me.Label26.TabIndex = 4
        Me.Label26.Text = "플러싱:"
        '
        'lbWaitTime
        '
        Me.lbWaitTime.AutoSize = True
        Me.lbWaitTime.Location = New System.Drawing.Point(80, 133)
        Me.lbWaitTime.Name = "lbWaitTime"
        Me.lbWaitTime.Size = New System.Drawing.Size(17, 12)
        Me.lbWaitTime.TabIndex = 8
        Me.lbWaitTime.Text = "--"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(13, 133)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(61, 12)
        Me.Label5.TabIndex = 3
        Me.Label5.Text = "동작 시간:"
        '
        'btnConnect
        '
        Me.btnConnect.Font = New System.Drawing.Font("굴림", 18.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnConnect.Location = New System.Drawing.Point(7, 81)
        Me.btnConnect.Name = "btnConnect"
        Me.btnConnect.Size = New System.Drawing.Size(101, 37)
        Me.btnConnect.TabIndex = 13
        Me.btnConnect.Text = "연결"
        Me.btnConnect.UseVisualStyleBackColor = True
        '
        'btnDisconnect
        '
        Me.btnDisconnect.Font = New System.Drawing.Font("굴림", 18.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnDisconnect.Location = New System.Drawing.Point(126, 81)
        Me.btnDisconnect.Name = "btnDisconnect"
        Me.btnDisconnect.Size = New System.Drawing.Size(101, 37)
        Me.btnDisconnect.TabIndex = 14
        Me.btnDisconnect.Text = "해제"
        Me.btnDisconnect.UseVisualStyleBackColor = True
        '
        'cmbPort
        '
        Me.cmbPort.FormattingEnabled = True
        Me.cmbPort.Location = New System.Drawing.Point(101, 20)
        Me.cmbPort.Name = "cmbPort"
        Me.cmbPort.Size = New System.Drawing.Size(121, 20)
        Me.cmbPort.TabIndex = 15
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(16, 23)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(79, 12)
        Me.Label6.TabIndex = 10
        Me.Label6.Text = "COM PORT :"
        '
        'cmbBaud
        '
        Me.cmbBaud.Enabled = False
        Me.cmbBaud.FormattingEnabled = True
        Me.cmbBaud.Location = New System.Drawing.Point(101, 50)
        Me.cmbBaud.Name = "cmbBaud"
        Me.cmbBaud.Size = New System.Drawing.Size(121, 20)
        Me.cmbBaud.TabIndex = 16
        '
        'SerialPort1
        '
        '
        'listBoxRead
        '
        Me.listBoxRead.FormattingEnabled = True
        Me.listBoxRead.ItemHeight = 12
        Me.listBoxRead.Location = New System.Drawing.Point(12, 498)
        Me.listBoxRead.Name = "listBoxRead"
        Me.listBoxRead.Size = New System.Drawing.Size(1029, 64)
        Me.listBoxRead.TabIndex = 17
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(16, 53)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(81, 12)
        Me.Label7.TabIndex = 18
        Me.Label7.Text = "BAUD RATE :"
        '
        'GroupBox9
        '
        Me.GroupBox9.Controls.Add(Me.btnDisconnect)
        Me.GroupBox9.Controls.Add(Me.btnConnect)
        Me.GroupBox9.Controls.Add(Me.cmbPort)
        Me.GroupBox9.Controls.Add(Me.Label6)
        Me.GroupBox9.Controls.Add(Me.cmbBaud)
        Me.GroupBox9.Controls.Add(Me.Label7)
        Me.GroupBox9.Location = New System.Drawing.Point(881, 15)
        Me.GroupBox9.Name = "GroupBox9"
        Me.GroupBox9.Size = New System.Drawing.Size(233, 127)
        Me.GroupBox9.TabIndex = 34
        Me.GroupBox9.TabStop = False
        Me.GroupBox9.Text = "통신"
        '
        'Timer1
        '
        Me.Timer1.Enabled = True
        Me.Timer1.Interval = 1000
        '
        'Timer2
        '
        Me.Timer2.Enabled = True
        Me.Timer2.Interval = 250
        '
        'GroupBox14
        '
        Me.GroupBox14.Controls.Add(Me.Label106)
        Me.GroupBox14.Controls.Add(Me.btnDown6)
        Me.GroupBox14.Controls.Add(Me.btnDown3)
        Me.GroupBox14.Controls.Add(Me.btnDown5)
        Me.GroupBox14.Controls.Add(Me.btnDown2)
        Me.GroupBox14.Controls.Add(Me.btnSaveSetting)
        Me.GroupBox14.Controls.Add(Me.btnDown4)
        Me.GroupBox14.Controls.Add(Me.btnDown1)
        Me.GroupBox14.Controls.Add(Me.btnUp6)
        Me.GroupBox14.Controls.Add(Me.btnUp3)
        Me.GroupBox14.Controls.Add(Me.btnUp5)
        Me.GroupBox14.Controls.Add(Me.btnUp2)
        Me.GroupBox14.Controls.Add(Me.btnUp4)
        Me.GroupBox14.Controls.Add(Me.btnUp1)
        Me.GroupBox14.Controls.Add(Me.lbSetting3)
        Me.GroupBox14.Controls.Add(Me.lbSetting2)
        Me.GroupBox14.Controls.Add(Me.Label10)
        Me.GroupBox14.Controls.Add(Me.Label2)
        Me.GroupBox14.Controls.Add(Me.lbSetting4)
        Me.GroupBox14.Controls.Add(Me.Label121)
        Me.GroupBox14.Controls.Add(Me.lbSetting6)
        Me.GroupBox14.Controls.Add(Me.lbSetting1)
        Me.GroupBox14.Controls.Add(Me.Label4)
        Me.GroupBox14.Controls.Add(Me.lbSetting5)
        Me.GroupBox14.Controls.Add(Me.Label8)
        Me.GroupBox14.Location = New System.Drawing.Point(484, 285)
        Me.GroupBox14.Name = "GroupBox14"
        Me.GroupBox14.Size = New System.Drawing.Size(557, 207)
        Me.GroupBox14.TabIndex = 86
        Me.GroupBox14.TabStop = False
        Me.GroupBox14.Text = "설정 데이터"
        '
        'Label106
        '
        Me.Label106.AutoSize = True
        Me.Label106.Location = New System.Drawing.Point(9, 61)
        Me.Label106.Name = "Label106"
        Me.Label106.Size = New System.Drawing.Size(61, 12)
        Me.Label106.TabIndex = 0
        Me.Label106.Text = "잔압 방출:"
        '
        'btnDown6
        '
        Me.btnDown6.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnDown6.Location = New System.Drawing.Point(406, 131)
        Me.btnDown6.Name = "btnDown6"
        Me.btnDown6.Size = New System.Drawing.Size(49, 34)
        Me.btnDown6.TabIndex = 54
        Me.btnDown6.Text = "-"
        Me.btnDown6.UseVisualStyleBackColor = True
        '
        'btnDown3
        '
        Me.btnDown3.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnDown3.Location = New System.Drawing.Point(169, 131)
        Me.btnDown3.Name = "btnDown3"
        Me.btnDown3.Size = New System.Drawing.Size(49, 34)
        Me.btnDown3.TabIndex = 54
        Me.btnDown3.Text = "-"
        Me.btnDown3.UseVisualStyleBackColor = True
        '
        'btnDown5
        '
        Me.btnDown5.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnDown5.Location = New System.Drawing.Point(406, 91)
        Me.btnDown5.Name = "btnDown5"
        Me.btnDown5.Size = New System.Drawing.Size(49, 34)
        Me.btnDown5.TabIndex = 54
        Me.btnDown5.Text = "-"
        Me.btnDown5.UseVisualStyleBackColor = True
        '
        'btnDown2
        '
        Me.btnDown2.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnDown2.Location = New System.Drawing.Point(169, 91)
        Me.btnDown2.Name = "btnDown2"
        Me.btnDown2.Size = New System.Drawing.Size(49, 34)
        Me.btnDown2.TabIndex = 54
        Me.btnDown2.Text = "-"
        Me.btnDown2.UseVisualStyleBackColor = True
        '
        'btnSaveSetting
        '
        Me.btnSaveSetting.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnSaveSetting.Location = New System.Drawing.Point(461, 49)
        Me.btnSaveSetting.Name = "btnSaveSetting"
        Me.btnSaveSetting.Size = New System.Drawing.Size(79, 114)
        Me.btnSaveSetting.TabIndex = 54
        Me.btnSaveSetting.Text = "SAVE"
        Me.btnSaveSetting.UseVisualStyleBackColor = True
        '
        'btnDown4
        '
        Me.btnDown4.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnDown4.Location = New System.Drawing.Point(406, 50)
        Me.btnDown4.Name = "btnDown4"
        Me.btnDown4.Size = New System.Drawing.Size(49, 34)
        Me.btnDown4.TabIndex = 54
        Me.btnDown4.Text = "-"
        Me.btnDown4.UseVisualStyleBackColor = True
        '
        'btnDown1
        '
        Me.btnDown1.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnDown1.Location = New System.Drawing.Point(169, 50)
        Me.btnDown1.Name = "btnDown1"
        Me.btnDown1.Size = New System.Drawing.Size(49, 34)
        Me.btnDown1.TabIndex = 54
        Me.btnDown1.Text = "-"
        Me.btnDown1.UseVisualStyleBackColor = True
        '
        'btnUp6
        '
        Me.btnUp6.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnUp6.Location = New System.Drawing.Point(351, 131)
        Me.btnUp6.Name = "btnUp6"
        Me.btnUp6.Size = New System.Drawing.Size(49, 34)
        Me.btnUp6.TabIndex = 54
        Me.btnUp6.Text = "+"
        Me.btnUp6.UseVisualStyleBackColor = True
        '
        'btnUp3
        '
        Me.btnUp3.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnUp3.Location = New System.Drawing.Point(114, 131)
        Me.btnUp3.Name = "btnUp3"
        Me.btnUp3.Size = New System.Drawing.Size(49, 34)
        Me.btnUp3.TabIndex = 54
        Me.btnUp3.Text = "+"
        Me.btnUp3.UseVisualStyleBackColor = True
        '
        'btnUp5
        '
        Me.btnUp5.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnUp5.Location = New System.Drawing.Point(351, 91)
        Me.btnUp5.Name = "btnUp5"
        Me.btnUp5.Size = New System.Drawing.Size(49, 34)
        Me.btnUp5.TabIndex = 54
        Me.btnUp5.Text = "+"
        Me.btnUp5.UseVisualStyleBackColor = True
        '
        'btnUp2
        '
        Me.btnUp2.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnUp2.Location = New System.Drawing.Point(114, 91)
        Me.btnUp2.Name = "btnUp2"
        Me.btnUp2.Size = New System.Drawing.Size(49, 34)
        Me.btnUp2.TabIndex = 54
        Me.btnUp2.Text = "+"
        Me.btnUp2.UseVisualStyleBackColor = True
        '
        'btnUp4
        '
        Me.btnUp4.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnUp4.Location = New System.Drawing.Point(351, 50)
        Me.btnUp4.Name = "btnUp4"
        Me.btnUp4.Size = New System.Drawing.Size(49, 34)
        Me.btnUp4.TabIndex = 54
        Me.btnUp4.Text = "+"
        Me.btnUp4.UseVisualStyleBackColor = True
        '
        'btnUp1
        '
        Me.btnUp1.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnUp1.Location = New System.Drawing.Point(114, 50)
        Me.btnUp1.Name = "btnUp1"
        Me.btnUp1.Size = New System.Drawing.Size(49, 34)
        Me.btnUp1.TabIndex = 54
        Me.btnUp1.Text = "+"
        Me.btnUp1.UseVisualStyleBackColor = True
        '
        'lbSetting3
        '
        Me.lbSetting3.AutoSize = True
        Me.lbSetting3.Location = New System.Drawing.Point(76, 142)
        Me.lbSetting3.Name = "lbSetting3"
        Me.lbSetting3.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting3.TabIndex = 8
        Me.lbSetting3.Text = "--"
        '
        'lbSetting2
        '
        Me.lbSetting2.AutoSize = True
        Me.lbSetting2.Location = New System.Drawing.Point(76, 102)
        Me.lbSetting2.Name = "lbSetting2"
        Me.lbSetting2.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting2.TabIndex = 8
        Me.lbSetting2.Text = "--"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(10, 142)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(60, 12)
        Me.Label10.TabIndex = 3
        Me.Label10.Text = " AIR 압축:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(241, 61)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(61, 12)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "입수 차단:"
        '
        'lbSetting4
        '
        Me.lbSetting4.AutoSize = True
        Me.lbSetting4.Location = New System.Drawing.Point(308, 61)
        Me.lbSetting4.Name = "lbSetting4"
        Me.lbSetting4.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting4.TabIndex = 8
        Me.lbSetting4.Text = "--"
        '
        'Label121
        '
        Me.Label121.AutoSize = True
        Me.Label121.Location = New System.Drawing.Point(9, 104)
        Me.Label121.Name = "Label121"
        Me.Label121.Size = New System.Drawing.Size(56, 12)
        Me.Label121.TabIndex = 3
        Me.Label121.Text = "AIR 유입:"
        '
        'lbSetting6
        '
        Me.lbSetting6.AutoSize = True
        Me.lbSetting6.Location = New System.Drawing.Point(308, 142)
        Me.lbSetting6.Name = "lbSetting6"
        Me.lbSetting6.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting6.TabIndex = 8
        Me.lbSetting6.Text = "--"
        '
        'lbSetting1
        '
        Me.lbSetting1.AutoSize = True
        Me.lbSetting1.Location = New System.Drawing.Point(76, 61)
        Me.lbSetting1.Name = "lbSetting1"
        Me.lbSetting1.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting1.TabIndex = 5
        Me.lbSetting1.Text = "--"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(257, 102)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(45, 12)
        Me.Label4.TabIndex = 3
        Me.Label4.Text = "역세척:"
        '
        'lbSetting5
        '
        Me.lbSetting5.AutoSize = True
        Me.lbSetting5.Location = New System.Drawing.Point(308, 102)
        Me.lbSetting5.Name = "lbSetting5"
        Me.lbSetting5.Size = New System.Drawing.Size(17, 12)
        Me.lbSetting5.TabIndex = 8
        Me.lbSetting5.Text = "--"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(241, 142)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(61, 12)
        Me.Label8.TabIndex = 3
        Me.Label8.Text = "연속 통수:"
        '
        'PictureBox1
        '
        Me.PictureBox1.Image = Global.ReFilter.My.Resources.Resources.reverse_filter
        Me.PictureBox1.Location = New System.Drawing.Point(12, 15)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(863, 264)
        Me.PictureBox1.TabIndex = 19
        Me.PictureBox1.TabStop = False
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.lbStep8)
        Me.GroupBox1.Controls.Add(Me.lbStep7)
        Me.GroupBox1.Controls.Add(Me.lbStep6)
        Me.GroupBox1.Controls.Add(Me.lbStep5)
        Me.GroupBox1.Controls.Add(Me.lbStep4)
        Me.GroupBox1.Controls.Add(Me.lbStep3)
        Me.GroupBox1.Controls.Add(Me.lbStep2)
        Me.GroupBox1.Controls.Add(Me.lbStep1)
        Me.GroupBox1.Controls.Add(Me.lbRepeat)
        Me.GroupBox1.Controls.Add(Me.Label9)
        Me.GroupBox1.Controls.Add(Me.btnReset)
        Me.GroupBox1.Location = New System.Drawing.Point(220, 285)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(258, 207)
        Me.GroupBox1.TabIndex = 87
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "동작 상태"
        '
        'lbStep8
        '
        Me.lbStep8.AutoSize = True
        Me.lbStep8.Location = New System.Drawing.Point(166, 93)
        Me.lbStep8.Name = "lbStep8"
        Me.lbStep8.Size = New System.Drawing.Size(43, 12)
        Me.lbStep8.TabIndex = 4
        Me.lbStep8.Text = "8. 완료"
        '
        'lbStep7
        '
        Me.lbStep7.AutoSize = True
        Me.lbStep7.Location = New System.Drawing.Point(166, 70)
        Me.lbStep7.Name = "lbStep7"
        Me.lbStep7.Size = New System.Drawing.Size(71, 12)
        Me.lbStep7.TabIndex = 4
        Me.lbStep7.Text = "7. 연속 통수"
        '
        'lbStep6
        '
        Me.lbStep6.AutoSize = True
        Me.lbStep6.Location = New System.Drawing.Point(166, 47)
        Me.lbStep6.Name = "lbStep6"
        Me.lbStep6.Size = New System.Drawing.Size(59, 12)
        Me.lbStep6.TabIndex = 4
        Me.lbStep6.Text = "6. 역세척 "
        '
        'lbStep5
        '
        Me.lbStep5.AutoSize = True
        Me.lbStep5.Location = New System.Drawing.Point(166, 24)
        Me.lbStep5.Name = "lbStep5"
        Me.lbStep5.Size = New System.Drawing.Size(71, 12)
        Me.lbStep5.TabIndex = 4
        Me.lbStep5.Text = "5. 입수 차단"
        '
        'lbStep4
        '
        Me.lbStep4.AutoSize = True
        Me.lbStep4.Location = New System.Drawing.Point(11, 93)
        Me.lbStep4.Name = "lbStep4"
        Me.lbStep4.Size = New System.Drawing.Size(106, 12)
        Me.lbStep4.TabIndex = 4
        Me.lbStep4.Text = "4. 공필터 AIR 압축"
        '
        'lbStep3
        '
        Me.lbStep3.AutoSize = True
        Me.lbStep3.Location = New System.Drawing.Point(11, 70)
        Me.lbStep3.Name = "lbStep3"
        Me.lbStep3.Size = New System.Drawing.Size(106, 12)
        Me.lbStep3.TabIndex = 4
        Me.lbStep3.Text = "3. 공필터 AIR 유입"
        '
        'lbStep2
        '
        Me.lbStep2.AutoSize = True
        Me.lbStep2.Location = New System.Drawing.Point(11, 47)
        Me.lbStep2.Name = "lbStep2"
        Me.lbStep2.Size = New System.Drawing.Size(111, 12)
        Me.lbStep2.TabIndex = 5
        Me.lbStep2.Text = "2. 공필터 잔압 배출"
        '
        'lbStep1
        '
        Me.lbStep1.AutoSize = True
        Me.lbStep1.Location = New System.Drawing.Point(11, 24)
        Me.lbStep1.Name = "lbStep1"
        Me.lbStep1.Size = New System.Drawing.Size(83, 12)
        Me.lbStep1.TabIndex = 6
        Me.lbStep1.Text = "1. 초기 플러싱"
        '
        'lbRepeat
        '
        Me.lbRepeat.AutoSize = True
        Me.lbRepeat.Location = New System.Drawing.Point(76, 133)
        Me.lbRepeat.Name = "lbRepeat"
        Me.lbRepeat.Size = New System.Drawing.Size(17, 12)
        Me.lbRepeat.TabIndex = 8
        Me.lbRepeat.Text = "--"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(9, 133)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(61, 12)
        Me.Label9.TabIndex = 3
        Me.Label9.Text = "동작 횟수:"
        '
        'btnReset
        '
        Me.btnReset.Font = New System.Drawing.Font("굴림", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(129, Byte))
        Me.btnReset.Location = New System.Drawing.Point(11, 154)
        Me.btnReset.Name = "btnReset"
        Me.btnReset.Size = New System.Drawing.Size(241, 47)
        Me.btnReset.TabIndex = 54
        Me.btnReset.Text = "동작 횟수 초기화"
        Me.btnReset.UseVisualStyleBackColor = True
        '
        'Form
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(1129, 568)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.PictureBox1)
        Me.Controls.Add(Me.GroupBox14)
        Me.Controls.Add(Me.GroupBox9)
        Me.Controls.Add(Me.listBoxRead)
        Me.Controls.Add(Me.GroupBox3)
        Me.MaximizeBox = False
        Me.MaximumSize = New System.Drawing.Size(1349, 692)
        Me.MinimizeBox = False
        Me.Name = "Form"
        Me.Text = "코웨이-선행개발팀-필터 역세척 시스템 "
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.GroupBox9.ResumeLayout(False)
        Me.GroupBox9.PerformLayout()
        Me.GroupBox14.ResumeLayout(False)
        Me.GroupBox14.PerformLayout()
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox3 As GroupBox
    Friend WithEvents lbValveFlushing As Label
    Friend WithEvents Label16 As Label
    Friend WithEvents lbValveFeed As Label
    Friend WithEvents Label26 As Label
    Friend WithEvents btnConnect As Button
    Friend WithEvents btnDisconnect As Button
    Friend WithEvents cmbPort As ComboBox
    Friend WithEvents Label6 As Label
    Friend WithEvents cmbBaud As ComboBox
    Friend WithEvents SerialPort1 As IO.Ports.SerialPort
    Friend WithEvents listBoxRead As ListBox
    Friend WithEvents Label7 As Label
    Friend WithEvents GroupBox9 As GroupBox
    Friend WithEvents Timer1 As Timer
    Friend WithEvents BackgroundWorker1 As System.ComponentModel.BackgroundWorker
    Friend WithEvents Timer2 As Timer
    Friend WithEvents lbValveOut As Label
    Friend WithEvents Label22 As Label
    Friend WithEvents GroupBox14 As GroupBox
    Friend WithEvents Label106 As Label
    Friend WithEvents lbSetting2 As Label
    Friend WithEvents Label121 As Label
    Friend WithEvents lbSetting1 As Label
    Friend WithEvents PictureBox1 As PictureBox
    Friend WithEvents lbPumpFeedback As Label
    Friend WithEvents lbPumpOnOff As Label
    Friend WithEvents Label1 As Label
    Friend WithEvents btnDown6 As Button
    Friend WithEvents btnDown3 As Button
    Friend WithEvents btnDown5 As Button
    Friend WithEvents btnDown2 As Button
    Friend WithEvents btnDown4 As Button
    Friend WithEvents btnDown1 As Button
    Friend WithEvents btnUp6 As Button
    Friend WithEvents btnUp3 As Button
    Friend WithEvents btnUp5 As Button
    Friend WithEvents btnUp2 As Button
    Friend WithEvents btnUp4 As Button
    Friend WithEvents btnUp1 As Button
    Friend WithEvents lbSetting3 As Label
    Friend WithEvents Label10 As Label
    Friend WithEvents Label2 As Label
    Friend WithEvents lbSetting4 As Label
    Friend WithEvents lbSetting6 As Label
    Friend WithEvents Label4 As Label
    Friend WithEvents lbSetting5 As Label
    Friend WithEvents Label8 As Label
    Friend WithEvents lbWaitTime As Label
    Friend WithEvents Label5 As Label
    Friend WithEvents btnSaveSetting As Button
    Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents lbStep3 As Label
    Friend WithEvents lbStep2 As Label
    Friend WithEvents lbStep1 As Label
    Friend WithEvents lbRepeat As Label
    Friend WithEvents Label9 As Label
    Friend WithEvents btnSkip As Button
    Friend WithEvents btnReset As Button
    Friend WithEvents lbStep8 As Label
    Friend WithEvents lbStep7 As Label
    Friend WithEvents lbStep6 As Label
    Friend WithEvents lbStep5 As Label
    Friend WithEvents lbStep4 As Label
End Class
