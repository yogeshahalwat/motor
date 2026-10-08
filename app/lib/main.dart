import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'firebase_options.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(options: DefaultFirebaseOptions.currentPlatform);
  runApp(const MotorControllerApp());
}

class MotorControllerApp extends StatelessWidget {
  const MotorControllerApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Motor Controller',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorSchemeSeed: Colors.green,
        useMaterial3: true,
        brightness: Brightness.light,
      ),
      home: const AuthGate(),
    );
  }
}

class AuthGate extends StatelessWidget {
  const AuthGate({super.key});

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<User?>(
      stream: FirebaseAuth.instance.authStateChanges(),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Scaffold(
            body: Center(child: CircularProgressIndicator()),
          );
        }
        if (snapshot.hasData) {
          return const MotorControlScreen();
        }
        return const LoginScreen();
      },
    );
  }
}

// ==================== LOGIN SCREEN ====================

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final _phoneController = TextEditingController();
  final _otpController = TextEditingController();
  String? _verificationId;
  bool _otpSent = false;
  bool _loading = false;
  String? _error;

  Future<void> _sendOTP() async {
    setState(() { _loading = true; _error = null; });

    String phone = _phoneController.text.trim();
    if (!phone.startsWith('+')) {
      phone = '+91$phone';
    }

    await FirebaseAuth.instance.verifyPhoneNumber(
      phoneNumber: phone,
      timeout: const Duration(seconds: 60),
      verificationCompleted: (PhoneAuthCredential credential) async {
        await FirebaseAuth.instance.signInWithCredential(credential);
      },
      verificationFailed: (FirebaseAuthException e) {
        setState(() { _loading = false; _error = e.message; });
      },
      codeSent: (String verificationId, int? resendToken) {
        setState(() {
          _verificationId = verificationId;
          _otpSent = true;
          _loading = false;
        });
      },
      codeAutoRetrievalTimeout: (String verificationId) {
        _verificationId = verificationId;
      },
    );
  }

  Future<void> _verifyOTP() async {
    setState(() { _loading = true; _error = null; });

    try {
      final credential = PhoneAuthProvider.credential(
        verificationId: _verificationId!,
        smsCode: _otpController.text.trim(),
      );
      await FirebaseAuth.instance.signInWithCredential(credential);
    } on FirebaseAuthException catch (e) {
      setState(() { _loading = false; _error = e.message; });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              const Icon(Icons.electric_bolt, size: 80, color: Colors.green),
              const SizedBox(height: 16),
              const Text(
                'Motor Controller',
                textAlign: TextAlign.center,
                style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
              ),
              const SizedBox(height: 8),
              const Text(
                'Login with your phone number',
                textAlign: TextAlign.center,
                style: TextStyle(fontSize: 16, color: Colors.grey),
              ),
              const SizedBox(height: 40),

              if (!_otpSent) ...[
                TextField(
                  controller: _phoneController,
                  keyboardType: TextInputType.phone,
                  decoration: const InputDecoration(
                    labelText: 'Phone Number',
                    prefixText: '+91 ',
                    border: OutlineInputBorder(),
                    prefixIcon: Icon(Icons.phone),
                  ),
                ),
                const SizedBox(height: 16),
                FilledButton(
                  onPressed: _loading ? null : _sendOTP,
                  child: _loading
                      ? const SizedBox(height: 20, width: 20, child: CircularProgressIndicator(strokeWidth: 2, color: Colors.white))
                      : const Text('Send OTP'),
                ),
              ],

              if (_otpSent) ...[
                TextField(
                  controller: _otpController,
                  keyboardType: TextInputType.number,
                  maxLength: 6,
                  decoration: const InputDecoration(
                    labelText: 'Enter OTP',
                    border: OutlineInputBorder(),
                    prefixIcon: Icon(Icons.lock),
                  ),
                ),
                const SizedBox(height: 16),
                FilledButton(
                  onPressed: _loading ? null : _verifyOTP,
                  child: _loading
                      ? const SizedBox(height: 20, width: 20, child: CircularProgressIndicator(strokeWidth: 2, color: Colors.white))
                      : const Text('Verify & Login'),
                ),
                TextButton(
                  onPressed: () => setState(() { _otpSent = false; }),
                  child: const Text('Change phone number'),
                ),
              ],

              if (_error != null) ...[
                const SizedBox(height: 16),
                Text(_error!, style: const TextStyle(color: Colors.red), textAlign: TextAlign.center),
              ],
            ],
          ),
        ),
      ),
    );
  }

  @override
  void dispose() {
    _phoneController.dispose();
    _otpController.dispose();
    super.dispose();
  }
}

// ==================== MOTOR CONTROL SCREEN ====================

class MotorControlScreen extends StatefulWidget {
  const MotorControlScreen({super.key});

  @override
  State<MotorControlScreen> createState() => _MotorControlScreenState();
}

class _MotorControlScreenState extends State<MotorControlScreen> {
  final DatabaseReference _deviceRef =
      FirebaseDatabase.instance.ref('devices/test_device_001');

  String _command = 'OFF';
  bool _motorRunning = false;
  bool _fault = false;
  bool _allOk = false;
  bool _deviceOnline = false;
  DateTime? _lastSeen;

  @override
  void initState() {
    super.initState();
    _listenToDevice();
  }

  void _listenToDevice() {
    _deviceRef.onValue.listen((event) {
      if (event.snapshot.exists) {
        final data = event.snapshot.value as Map<dynamic, dynamic>?;
        if (data != null) {
          setState(() {
            _command = (data['command'] ?? 'OFF').toString();
            _motorRunning = data['motor_running'] == true || data['motor_running'] == 'true';
            _fault = data['fault'] == true || data['fault'] == 'true';
            _allOk = data['all_ok'] == true || data['all_ok'] == 'true';
            final lastSeenMs = data['last_seen'];
            if (lastSeenMs != null) {
              _lastSeen = DateTime.fromMillisecondsSinceEpoch(int.tryParse(lastSeenMs.toString()) ?? 0);
              final diff = DateTime.now().difference(_lastSeen!);
              _deviceOnline = diff.inMinutes < 5;
            }
          });
        }
      }
    });
  }

  Future<void> _toggleMotor() async {
    final newCommand = _command == 'ON' ? 'OFF' : 'ON';
    await _deviceRef.update({
      'command': newCommand,
      'command_timestamp': ServerValue.timestamp,
      'command_source': 'app',
    });
  }

  Future<void> _signOut() async {
    await FirebaseAuth.instance.signOut();
  }

  @override
  Widget build(BuildContext context) {
    final isOn = _command == 'ON';

    return Scaffold(
      appBar: AppBar(
        title: const Text('Motor Controller'),
        actions: [
          IconButton(
            icon: const Icon(Icons.logout),
            onPressed: _signOut,
            tooltip: 'Logout',
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(24.0),
        child: Column(
          children: [
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Row(
                  children: [
                    Icon(
                      Icons.circle,
                      size: 12,
                      color: _deviceOnline ? Colors.green : Colors.grey,
                    ),
                    const SizedBox(width: 8),
                    Text(
                      _deviceOnline ? 'Device Online' : 'Device Offline',
                      style: TextStyle(
                        color: _deviceOnline ? Colors.green : Colors.grey,
                        fontWeight: FontWeight.w500,
                      ),
                    ),
                    const Spacer(),
                    if (_lastSeen != null)
                      Text(
                        'Last seen: ${_formatTime(_lastSeen!)}',
                        style: const TextStyle(fontSize: 12, color: Colors.grey),
                      ),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 24),

            GestureDetector(
              onTap: _toggleMotor,
              child: Container(
                width: 200,
                height: 200,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: isOn ? Colors.green : Colors.grey.shade300,
                  boxShadow: [
                    BoxShadow(
                      color: isOn
                          ? Colors.green.withValues(alpha: 0.4)
                          : Colors.grey.withValues(alpha: 0.3),
                      blurRadius: 30,
                      spreadRadius: 5,
                    ),
                  ],
                ),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Icon(
                      Icons.power_settings_new,
                      size: 64,
                      color: isOn ? Colors.white : Colors.grey.shade600,
                    ),
                    const SizedBox(height: 8),
                    Text(
                      isOn ? 'ON' : 'OFF',
                      style: TextStyle(
                        fontSize: 24,
                        fontWeight: FontWeight.bold,
                        color: isOn ? Colors.white : Colors.grey.shade600,
                      ),
                    ),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 12),
            Text(
              'Tap to turn ${isOn ? 'OFF' : 'ON'}',
              style: const TextStyle(color: Colors.grey),
            ),

            const SizedBox(height: 32),

            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                _StatusIndicator(
                  label: 'Motor',
                  icon: Icons.settings,
                  isActive: _motorRunning,
                  activeColor: Colors.green,
                ),
                _StatusIndicator(
                  label: 'Fault',
                  icon: Icons.warning,
                  isActive: _fault,
                  activeColor: Colors.red,
                ),
                _StatusIndicator(
                  label: 'All OK',
                  icon: Icons.check_circle,
                  isActive: _allOk,
                  activeColor: Colors.blue,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  String _formatTime(DateTime time) {
    final now = DateTime.now();
    final diff = now.difference(time);
    if (diff.inSeconds < 60) return '${diff.inSeconds}s ago';
    if (diff.inMinutes < 60) return '${diff.inMinutes}m ago';
    if (diff.inHours < 24) return '${diff.inHours}h ago';
    return '${diff.inDays}d ago';
  }
}

class _StatusIndicator extends StatelessWidget {
  final String label;
  final IconData icon;
  final bool isActive;
  final Color activeColor;

  const _StatusIndicator({
    required this.label,
    required this.icon,
    required this.isActive,
    required this.activeColor,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Container(
          width: 64,
          height: 64,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            color: isActive
                ? activeColor.withValues(alpha: 0.15)
                : Colors.grey.shade100,
          ),
          child: Icon(
            icon,
            size: 32,
            color: isActive ? activeColor : Colors.grey,
          ),
        ),
        const SizedBox(height: 8),
        Text(
          label,
          style: TextStyle(
            fontSize: 12,
            fontWeight: FontWeight.w500,
            color: isActive ? activeColor : Colors.grey,
          ),
        ),
      ],
    );
  }
}
