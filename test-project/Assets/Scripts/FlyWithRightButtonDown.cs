using UnityEngine;
using System.Collections;

// Fly around with typical FPS style controls, when the right mouse button is pressed
// Loosely based on the standard assets MouseLook.cs and FPSWalker.js
public class FlyWithRightButtonDown : MonoBehaviour
{
	// Settings
	public float mouseSensitivity = 10F;
	public float moveSpeed = 3F;
	
	// ivars
	float rotationX = 0F;
	float rotationY = 0F;
	Quaternion originalRotation;
	Vector3 moveDirection = Vector3.zero;

	void Start ()
	{
		originalRotation = transform.localRotation;
	}

	// Update is called once per frame
	void Update ()
	{
		// Only when the right mouse button is pressed
		if(!Input.GetMouseButton(1))
			return;

		// Rotate using the mouse axes
		rotationX += Input.GetAxis("Mouse X") * mouseSensitivity;
		rotationY += Input.GetAxis("Mouse Y") * mouseSensitivity;

		Quaternion xQuaternion = Quaternion.AngleAxis (rotationX, Vector3.up);
		Quaternion yQuaternion = Quaternion.AngleAxis (rotationY, Vector3.left);
		
		transform.localRotation = originalRotation * xQuaternion * yQuaternion;
		
		// Move using standard movement controls
		moveDirection = new Vector3(Input.GetAxis("Horizontal"), 0, Input.GetAxis("Vertical"));
		moveDirection *= moveSpeed;
		transform.Translate(moveDirection);
	}
}
